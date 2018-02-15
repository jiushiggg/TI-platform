#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include "mini_fs.h"
#include "lattice_epd/scrn_public_fun.h"

#define FILE_SYS_FLAG     EPD_3_VERSION
#define SWAP_ADDR	((WORD)(SEGMENT_SIZE * (FS_BLOCK + SUPER_BLOCK)))
#define SUPER_ADDR	((WORD)(SEGMENT_SIZE * FS_BLOCK + EF_STAA_DDR))

fs_t fs;
enum {
    FS_FLAG_CHANGED	= 0x01,
    FS_FLAG_SWAP_CLEAN	= 0x02,
    FS_FLAG_SWAP_DIRE	= 0x04,	//����segment_to_segment������1ʱ��ʾ�����ݴ�SWAP������DISK
};

enum {
    NORMAL_WRITE,
    DIRECT_WRITE,
};

static void segment_clean(WORD addr, WORD noused, WORD len);
typedef void (*op_fun_t)(WORD addr, WORD data, WORD len);
static void addr_split_opera(WORD addr, WORD data, WORD len, op_fun_t op);

#define VIRT2PHY(virt) (DISK+virt)

/*******************************************************
 ***	�û��ӿڲ����
 ***	read����ֱ�ӷ���FLASH��ַ����
 *** 	write������Ҫ����FLASH��д�������
 ***	erase��������ָ�����ļ�
 *******************************************************/

WORD f_len(file_id_t id) {
    return id >= FILE_ID_END ? 0 : fs.fs_len[id];
}

WORD f_size(file_id_t id) {
    return id >= FILE_ID_END ? 0 : fs.fs_ext[id].file_size;
}

WORD f_addr(file_id_t id) {
    return id >= FILE_ID_END ? 0 : fs.fs_ext[id].start_addr;
}

#ifdef FS_DISK_ROM_FLASH
const uint8_t* f_rom_read(file_id_t id, WORD offset) {
    if (id >= FILE_ID_END || offset >= f_size(id))
        return NULL;
    return (const uint8_t *)(VIRT2PHY(f_addr(id) + offset));
}
#endif


static int check_args(file_id_t id, WORD offset, const uint8_t *buf, WORD len) {
    WORD size;
    if (id >= FILE_ID_END || len == 0 || buf == NULL || fs.valid != FILE_SYS_FLAG)
        return 0;

    size = f_size(id);
    if (len > size || offset >= size || offset + len < len || offset + len < offset)
        return 0;

    if (offset + len > size)
        len = size - offset;
    return len;
}

static void segment_clean(WORD addr, WORD noused, WORD len)
{
#ifdef HAVE_EX_FLASH
    segment_erase(addr);
#else
    rom_segment_erase(addr);
#endif
}

static void ex_flash_or_rom_write(WORD addr, WORD data, WORD len)
{
#ifdef HAVE_EX_FLASH
    segment_write(addr,data,len);
#else
    rom_segment_write(addr,data,len);
#endif
}

WORD f_read(file_id_t id, WORD offset,	uint8_t *buf, WORD len)
{
    if ((len = check_args(id, offset, buf, len)) == 0)
        return 0;

#ifdef HAVE_EX_FLASH
    segment_read(VIRT2PHY(f_addr(id) + offset), (WORD)buf, len);
#else
    rom_segment_read(VIRT2PHY(f_addr(id) + offset), (WORD)buf, len);
#endif
    return len;
}

static WORD _f_write(file_id_t id,	WORD offset, const uint8_t *data, WORD len, uint8_t write_flag)
{
    WORD n, file_addr, file_len;

    if ((len = check_args(id, offset, data, len)) == 0)
        return 0;
    //ÿ��д�ļ�ʱ���������Ҫ�޸ĵĲ��ֺ���Ҫ׷�ӵĲ���
    //��Ҫ�޸ĵĲ�����Ƶ�����FLASH����׷�ӵĲ��������ļ�����ʱ�Ѿ���д����
    file_addr = f_addr(id);
    file_len = f_len(id);

    if (offset >= file_len)
    {
        //��Ҫ���������ֻ��Ҫ׷��
        fs.fs_len[id] = offset + len;
    } else if (offset < file_len && offset + len > file_len)
    {
        //��Ҫ���������һ����λ�����������ڲ�������һ������Ҫ׷��
        n = file_len - offset;
        if (write_flag != DIRECT_WRITE)
            addr_split_opera(VIRT2PHY(file_addr + offset), (WORD)data, n, segment_clean);
        fs.fs_len[id] = offset + len;
    }else
    {
        //��Ҫ�����������ȫλ�����������ڲ�
        if (write_flag != DIRECT_WRITE)
            addr_split_opera(VIRT2PHY(file_addr + offset), (WORD)data, len, segment_clean);
    }
    addr_split_opera(VIRT2PHY(file_addr + offset), (WORD)data, len, (op_fun_t)ex_flash_or_rom_write);

    fs.flag |= FS_FLAG_CHANGED;

    return len;
}

WORD f_write(file_id_t id,	WORD offset, const uint8_t *data, WORD len)
{
    return _f_write(id, offset, data, len, NORMAL_WRITE);
}

WORD 	f_write_direct(file_id_t id, WORD offset,	const uint8_t *data, WORD len) {
    // return _f_write(id, offset, data, len, DIRECT_WRITE);
    WORD file_addr, file_len;
    if ((len = check_args(id, offset, data, len)) == 0)
        return 0;

    file_addr = f_addr(id);
    file_len = f_len(id);
    addr_split_opera(VIRT2PHY(file_addr + offset), (WORD)data, len, (op_fun_t)ex_flash_or_rom_write);

    if (offset + len > file_len)
        fs.fs_len[id] = offset + len;

    fs.flag |= FS_FLAG_CHANGED;

    return len;
}		

void fs_erase_all(void)
{

    UINT32 block_num = (EF_EADDR- EF_STAA_DDR)/EF_BLOCK_SIZE,i =0,offset=0;
    for(i= 0;i<block_num;i++)
    {
#ifdef HAVE_EX_FLASH
        segment_erase(EF_STAA_DDR+offset);
#else
        rom_segment_erase(EF_STAA_DDR+offset);
#endif

        offset += EF_BLOCK_SIZE;
    }
}

void fs_erase_all_file(void)
{

    UINT32 block_num = FLASH_ALL_SIZE/EF_BLOCK_SIZE,i =0,offset=0;
    for(i= 0;i<block_num;i++)
    {
#ifdef HAVE_EX_FLASH
        segment_erase(EF_STAA_DDR+offset);
#else
        rom_segment_erase(EF_STAA_DDR+offset);
#endif

        offset += EF_BLOCK_SIZE;
    }
}

void f_erase(file_id_t id) {
    //���ļ����ȵ���0ʱ��Ҳ������segment_erase����
    if ( id >= FILE_ID_END )
        return;
    addr_split_opera(VIRT2PHY(f_addr(id)), (WORD)NULL, f_size(id), segment_clean);
    fs.fs_len[id] = 0;

    fs.flag |= FS_FLAG_CHANGED;
}

void f_sync(void) {
    if (fs.flag & FS_FLAG_CHANGED)
    {
        fs.flag &= ~FS_FLAG_CHANGED;
        addr_split_opera(VIRT2PHY(SUPER_ADDR), (WORD)&fs, sizeof(fs), segment_clean);
        addr_split_opera(VIRT2PHY(SUPER_ADDR), (WORD)&fs, sizeof(fs), (op_fun_t)ex_flash_or_rom_write);
    }
}

void f_init_check(UINT8 tp)
{ 
    UINT8 i;
    for(i=0;i<tp;i++)
    {
#ifdef HAVE_EX_FLASH
        segment_read(VIRT2PHY(SUPER_ADDR), (WORD)&fs, sizeof(fs));
#else
        rom_segment_read(VIRT2PHY(SUPER_ADDR), (WORD)&fs, sizeof(fs));
#endif
        if (fs.valid == FILE_SYS_FLAG)
            break;

        i++;
    }
}

struct fs_info_ext_t fs_info_temp[FILE_ID_END]={0};
fs_t fs_temp = {
                .flag = 0,
                .fs_len = {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                .fs_ext = fs_info_temp,
};

void f_init(void) {

    UINT8 i;
    file_id_t id;

#ifdef HAVE_EX_FLASH
    UINT16  screen_file_size;

    screen_file_size = UP_ALIGN(high_wide.high * high_wide.wide/8);
    fs_info_temp[0].start_addr = FILE_START_ADDR;
    fs_info_temp[0].file_size  = screen_file_size;
    fs_info_temp[1].start_addr = FILE_START_ADDR + screen_file_size;
    fs_info_temp[1].file_size  = screen_file_size;
    fs_info_temp[2].start_addr = FILE_START_ADDR + screen_file_size + screen_file_size;
    fs_info_temp[2].file_size  = FLASH_SB;
    fs_info_temp[3].start_addr = FILE_START_ADDR + screen_file_size + screen_file_size + FLASH_SB;
    fs_info_temp[3].file_size  = FLASH_LAYER_MAP;
    fs_info_temp[4].start_addr = FILE_START_ADDR + screen_file_size + screen_file_size + FLASH_SB + FLASH_LAYER_MAP;
    fs_info_temp[4].file_size  = PKG_NUM_BITMAP;
    fs_info_temp[5].start_addr = FILE_START_ADDR + screen_file_size + screen_file_size + FLASH_SB + FLASH_LAYER_MAP + PKG_NUM_BITMAP;
    fs_info_temp[5].file_size  = PKG_NUM_BITMAP;
    fs_info_temp[6].start_addr = FILE_START_ADDR + screen_file_size + screen_file_size + FLASH_SB + FLASH_LAYER_MAP + PKG_NUM_BITMAP + PKG_NUM_BITMAP;
    fs_info_temp[6].file_size  = F_PAGE_INFO;
    fs_info_temp[7].start_addr = FILE_START_ADDR + screen_file_size + screen_file_size + FLASH_SB + FLASH_LAYER_MAP + PKG_NUM_BITMAP + PKG_NUM_BITMAP + F_PAGE_INFO;
    fs_info_temp[7].file_size  = F_PAGE_INFO;
    fs_info_temp[8].start_addr = FILE_START_ADDR + screen_file_size + screen_file_size + FLASH_SB + FLASH_LAYER_MAP + PKG_NUM_BITMAP + PKG_NUM_BITMAP + F_PAGE_INFO + F_PAGE_INFO;
    fs_info_temp[8].file_size  = F_PAGE_NUM;
    fs_info_temp[9].start_addr = FILE_START_ADDR + screen_file_size + screen_file_size + FLASH_SB + FLASH_LAYER_MAP + PKG_NUM_BITMAP + PKG_NUM_BITMAP + F_PAGE_INFO + F_PAGE_INFO + F_PAGE_NUM;
    fs_info_temp[9].file_size  = F_PAGE_NUM;
    fs_info_temp[10].start_addr = FILE_START_ADDR + screen_file_size + screen_file_size + FLASH_SB + FLASH_LAYER_MAP + PKG_NUM_BITMAP + PKG_NUM_BITMAP + F_PAGE_INFO + F_PAGE_INFO + F_PAGE_NUM + F_PAGE_NUM;
    fs_info_temp[10].file_size  = FLASH_ALL_SIZE- (FILE_START_ADDR + screen_file_size + screen_file_size + FLASH_SB + FLASH_LAYER_MAP + PKG_NUM_BITMAP + PKG_NUM_BITMAP + F_PAGE_INFO + F_PAGE_INFO + F_PAGE_NUM + F_PAGE_NUM);

#else

    UINT8 temp = 1;

    if(screen_dis_t.src_color == BLACK_WHITE_RED)
        temp = 2;
    file_num = (FLASH_ALL_SIZE / (temp * UP_ALIGN( high_wide.high * high_wide.wide/8)));
    for(i=0;i < file_num;i++)
    {
        fs_info_temp[i].start_addr = FILE_START_ADDR + UP_ALIGN(temp * high_wide.high * high_wide.wide/8)*i;
        fs_info_temp[i].file_size  = temp * UP_ALIGN(high_wide.high * high_wide.wide/8);
    }

#endif

    fs.fs_ext = fs_temp.fs_ext;         //ÿ�μ�ǩ����������Ҫ��������ļ�����ʼ��ַ���ļ���С

    if(fs.valid != FILE_SYS_FLAG)
    {
        gflash_empty = 0x80;
        memcpy(&fs, &fs_temp, sizeof(fs));
        fs_erase_all();
        memset(gSys_tp.page_map,0xff,8);//���ҳ��
        fs.valid = FILE_SYS_FLAG;
        fs.flag |= FS_FLAG_CHANGED;
    }else
    {
        for (id = FILE1; id < FILE_ID_END; id++)
        {
            if (f_len(id) > f_size(id))
            {
                gflash_empty = 0x80;
                f_erase(id);
            }
        }
    }
    f_sync();
}

/*******************************************************
 ***	�������������
 ***	��Ҫ�漰�������Ϳ����ʱ���������������Ĳ���
 *******************************************************/

//���������
static void addr_split_opera(WORD addr, WORD data, WORD len, op_fun_t op) {
    WORD i, temp_off, temp_len = len;
    WORD split_unit = SEGMENT_SIZE;
    //��һ��������δ����Ĳ���
    if ((temp_off = addr % split_unit) != 0) {
        if (temp_off + len > split_unit)
            temp_len = split_unit - temp_off;
        op(addr, data, temp_len);
        addr += temp_len; data += temp_len; len -= temp_len;
    }
    //�ڶ���, ����պö���Ĳ���
    for (i = len / split_unit; i > 0; i--, addr += split_unit, data += split_unit, len -= split_unit)
        op(addr, data, split_unit);
    //������,�������ʣ�µ�
    if (len != 0)
        op(addr, data, len);
}


#define is_contain(a, b, c,d ) ((d) >= (a) && (c) < (b))
#define MIN(a, b) (a) < (b) ? (a) : (b)
#define MAX(a, b) (a) > (b) ? (a) : (b)


/************************************************
 *****************��临�ƺ���********************
 ************************************************/

#define SWAP(a, b) do {\
        WORD temp; \
        if (fs.flag & FS_FLAG_SWAP_DIRE) {\
            temp = a; a = b; b = temp;\
        } \
} while (0);

#if defined(FS_DISK_ROM_FLASH) || defined(FS_DISK_RAM_FLASH)
static void data_to_swap(WORD swap_addr, WORD data_addr, WORD len) {
    SWAP(swap_addr, data_addr);
    segment_write(swap_addr, data_addr, len);
}
#endif

/*******************************************************
 ***	�ײ�IO����
 ***	��Ҫʵ��Flash�����������ڴ浽Flash�ĸ��ƣ�
 ***  flash��flash�ĸ��ƹ�3������
 *******************************************************/

#define DISK_SPACE   SEGMENT_SIZE*DISK_BLOCK

#define SIZEOF(s,m) ((size_t) sizeof(((s *)0)->m))

WORD DISK = 0;

/*******************************************
FILE1,
F_BMP_BW = FILE1,			//��Ļ��ʾ�ڰ���ɫ��������4k����
F_BMP_RED,                            //��Ļ��ʾ��ɫ��������4k����
F_SB,                                 //Ҫ��ʾ�Ľṹ�����Դ�С4k
F_LAY_MAP,                            //Ԫ��ƫ�Ƶ�ַ�������С4k
F_NUM_ATTR,                           //�洢���ָ�ʽ���ļ���С4k
F_BMP_PKG_1,                          //�������ݰ�����bitmap��С4k
F_BMP_PKG_2,                          //�������ݰ�����bitmap��С4k 
F_PAGE_INFO1,                         //8ҳ���ݵķ�number���ʹ洢����ÿҳ512���ֽڣ�256��Ԫ�أ���С4K
F_PAGE_INFO2,                         //8ҳ���ݵķ�number���ʹ洢��������ʹ��������С4K
F_PAGE_NUM_1,                         //8ҳ����Ҫ��ʾ�����֣�ÿ��ҳ��Ҫ��ʾ������512���ֽڣ�85�����֣���С4K
F_PAGE_NUM_2,                         //8ҳ����Ҫ��ʾ�����֣�ÿ��ҳ��Ҫ��ʾ������512���ֽڣ�85�����֣�����ʹ��������С4K
F_BMP_DATA,                           //Ԫ��bitmap�̶��������  
FILE_ID_END,

//�ܴ�С12k
#define FLASH_SB          (WORD)(4*1024)		                        //SB�ṹ����
#define FLASH_LAYER_MAP   (WORD)(4*1024)                                        //ͼ��ӳ����
#define FLASH_NUM_ATTR    (WORD)(4*1024)		                        //�洢���ָ�ʽ���ļ�
#define PKG_NUM_BITMAP    (WORD)(4*1024)                                        //ÿ�����ݰ�����bitmap��С 1.5k
#define F_PAGE_INFO       (WORD)(4*1024)                                        //
#define F_PAGE_NUM        (WORD)(4*1024)   

 *******************************************/
uint8_t max_page_fun(uint8_t file_sum)
{
#define SYS_SAVE_CHANGE_BUF_FILE_NUM 2

    if(file_sum < 2)
        return 0;

    if(file_sum == SYS_SAVE_CHANGE_BUF_FILE_NUM)
        return 1;

    file_sum -= SYS_SAVE_CHANGE_BUF_FILE_NUM;

    if(file_sum >=4)
        return 4;
    else
        return (file_sum/2*2);

}


