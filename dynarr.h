/**************************************
 *         Physics calculator
 *            CSR, 2014
 * http://info.dcsr.ru/projects/phcalc/
 * https://github.com/dhcsr/phcalc
 *
 * Dynamic array: header (dynarr.h)
 *
 **************************************/

void dynarr_create(void **arr, int elsize);
int dynarr_desable(void **arr);
int dynarr_length(void **arr);
void dynarr_add(void **arr, ...);