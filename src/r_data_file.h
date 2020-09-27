/*""FILE COMMENT""*******************************************************************
* File Name     : r_data_file.h
* Version       : 1.00
* Device(s)     : Renesas MCUs
* Tool-Chain    : -
* H/W Platform  : -
* Description   : File containing data to be written to the memory card
*************************************************************************************
* History	: DD.MM.YYYY Version Description
*""FILE COMMENT END""****************************************************************/
#ifndef _R_DATA_FILE_H_
#define	_R_DATA_FILE_H_

/************************************************************************************
Macro definitions
************************************************************************************/
#define FILESIZE	(2048u)			/*	2 KB data	*/

/************************************************************************************
Global variables
*************************************************************************************/
extern const uint8_t filebuffer[FILESIZE];

#endif	/* _R_DATA_FILE_H_ */
