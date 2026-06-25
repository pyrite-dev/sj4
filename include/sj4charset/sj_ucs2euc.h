#ifndef __SJ_UCS2EUC_H__
#define __SJ_UCS2EUC_H__

/*
 * file:utf16_to_euc_jp_table.h
 */
/*
# Unicode EUC-JP
# Character Code Mappings
#   <EUC-JP>
#       <制御コード>0x00～0x1f, 0x7f</制御コード>
#       <ASCII文字>0x20～0x7e</ASCII文字>
#       <半角カタカナ>0x8ea1～0x8edf</半角カタカナ>
#       <漢字>0xa1a1～0xf4a6, 0x8fa2af～0x8fede3<漢字>
#   </EUC-JP>
*/
struct _U2E_BITMAP_INDEX {
	unsigned char byType;	     /* 1:isEmpty 2:BitmapIndex 3:Data */
	unsigned long dwBitmapIndex; /* BitmapIndexへのポインタ(0の場合はBitmapIndexが存在しない) */
	unsigned long dwEucJpCode;   /* EUC_JP コード(未知の場合には0) */
	unsigned long dwUtf16Code;   /* UTF16コード(未知の場合には0) */
};
typedef struct _U2E_BITMAP_INDEX T_U2E_BITMAP_INDEX;

extern const T_U2E_BITMAP_INDEX utf16_to_euc_jp_table[];

#endif
