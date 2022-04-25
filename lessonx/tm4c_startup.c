// place at address mem: __ICFEDIT_intvec_start__ via IAR @ "<section>"
// make const for linker to place in ROM
const int __vector_table[] @ ".intvec" = {
    0x20004000,
    0x9
};