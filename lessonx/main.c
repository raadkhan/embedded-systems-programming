// standardizes type names and make compiler vendor define types for cpu
#include <stdint.h>

#include "tm4c_cmsis.h"
#include "delay.h"

#define RED_LED (0x1U << 1)
#define BLUE_LED (0x1U << 2)
#define GREEN_LED (0x1U << 3)

// tag name occupies different name space than typedef, func, var names
// MISRA advises not to have unused tag declarations
// __packed only if must avoid padding at cost of higher IC to access
typedef struct
{
    uint8_t y;
    // compiler inserts pad byte here
    uint16_t x;
} point; // not followed by list of vars so reserves no space until declaration

typedef struct
{
    point ptl;
    point pbr;
} window;

typedef struct
{
    point corns[3];
} triangle;

// C standard requires all initialized variables have initial values via
// initializer bytes in ROM to .data in RAM, and all uninitialized variables set
// to zero via .bss in RAM
int16_t x = -1;
uint32_t y = GREEN_LED | RED_LED;
int16_t sqr[] = {
    1 * 1,
    2 * 2,
    3 * 3,
    4 * 4
};

point p1 = {
    123U,
    0x1234U
};

window w1 = {
    {123U, 0x1234U},
    {234U, 0x2345U}
};

unsigned int fact(unsigned n);
int* swap(int* x, int* y);
void mix(void);
void structs(void);

// stress stack near/to breaking point
unsigned int fact(unsigned n)
{
    unsigned int foo[6];
    foo[n] = n;

    if (n == 0U)
    {
        return 1U;
    }
    else
    {
        return foo[n] * fact(n - 1U);
    }
}

int* swap(int* x, int* y)
{
    // static persists var in RAM and not on stack
    int static tmp[2];
    // arrays and pointers are compatible
    tmp[0] = *x;
    tmp[1] = *y;
    *x = tmp[1];
    *y = tmp[0];

    return tmp; // locals fall above stack so can get dirty by later func calls
}

void mix()
{
    uint8_t u8a, u8b;
    uint16_t u16c, u16d;
    uint32_t u32e, u32f;

    int8_t s8;
    int16_t s16;
    int32_t s32;

    // check type sizes
    u8a = sizeof(u8a);
    u16c = sizeof(uint16_t);
    u32e = sizeof(u32e);

    u8a = 0xa1U;        // B
    u16c = 0xc1c2U;     // H
    u32e = 0xe1e2e3e4U; // W

    // implicit conversion rules are complicated...
    u8b = u8a;
    u16d = u16c;
    u32f = u32e;
    // smaller size operands promoted to builtin int/uint in RHS expr
    // operands promoted to largest size operand in RHS expr
    u16c = 40000U;
    u16d = 30000U;
    u32e = (uint32_t)(u16c + u16d);
    // operands promoted to unsigned in mixed sign RHS expr
    // RHS expr result not sign extended for LHS expr
    u16c = 100U;
    s8 = 10;
    s32 = s8 - (int16_t)u16c;

    s16 = s32;

    // comparison always false without type cast
    if ((int32_t)u32f > s16)
        u8a = 1U;
    else
        u8a = 0U;

    u8b = 0xffU;
    // comparison always false without type cast
    if ((uint8_t)(~u8b) == 0x00U)
        u8b = 1U;
    else
        u8b = 0U;
}

void structs()
{
    point p1, p2;
    window w1, w2;
    triangle t1;

    // better to avoid struct-to-struct assignments
    point* pp;
    window* wp;

    p1.x = sizeof(point);
    p1.y = p1.x - 4U;

    w1.ptl.x = 1U;
    w1.pbr.y = 2U;

    t1.corns[0].x = 1U;
    t1.corns[1].y = t1.corns[0].x;
    t1.corns[2].x = 2U;

    p2 = p1;
    w2 = w1;

    pp = &p2;
    wp = &w2;

    (*pp).x = 1U;
    (*wp).pbr = *pp;
    pp->x = 1U;
    wp->pbr = *pp;
}

int main(void)
{
    // enable ahb
    // SYSCTL_GPIOHBCTL_R |= (0x1U << 5);
    SYSCTL->RCGC2 |= (0x1U << 5);

    // enable clock gate
    // SYSCTL_RCGCGPIO_R |= (0x1U << 5);
    SYSCTL->GPIOHSCTL |= (0x1U << 5);
    // GPIO_PORTF_AHB_DIR_R |= (RED_LED | BLUE_LED | GREEN_LED);
    GPIOF_HS->DIR |= (RED_LED | BLUE_LED | GREEN_LED);
    // GPIO_PORTF_AHB_DEN_R |= (RED_LED | BLUE_LED | GREEN_LED);
    GPIOF_HS->DEN |= (RED_LED | BLUE_LED | GREEN_LED);

    // multiple ways to set bits
    // GPIO_PORTF_DATA_R |= BLUE_LED;
    // address arithmetic requires offset scale based on data type
    //*((volatile unsigned long int*)(0x40025000U + (BLUE_LED << 2))) = BLUE_LED;
    // pointer arithmetic does not require offset scale
    //*(GPIO_PORTF_DATA_BITS_R + BLUE_LED) = BLUE_LED;
    // array does not require any arithmetic
    // GPIO_PORTF_AHB_DATA_BITS_R[BLUE_LED] = BLUE_LED;
    GPIOF_HS->DATA_Bits[BLUE_LED] = BLUE_LED;

    while (1)
    {
        int x = 1000000U;
        int y = 1000000U / 2U;
        int* z = swap(&x, &y);

        delay(z[0]); // argument type casted implicitly by func

        // GPIO_PORTF_AHB_DATA_BITS_R[RED_LED] = RED_LED;
        GPIOF_HS->DATA_Bits[RED_LED] = RED_LED;

        delay(z[1]); // argument gets type casted implicitly

        // multiple ways to clear bits
        // GPIO_PORTF_DATA_R &= ~RED_LED; // compiler knows how to optimize this
        // GPIO_PORTF_AHB_DATA_BITS_R[RED_LED] = 0;
        GPIOF_HS->DATA_Bits[RED_LED] = 0;
    }

    // C standard requires main to return int and
    // all non-void type return functions to explicity return int
    // breaks C standard but removes compiler warning
    // return 0;
}
