#include "cpu.h"

/* sets a single bit of one of the eight bit 
 * registers on CPU to val */
void setRegBit(CPU *c, REG name, int8_t bit, int8_t val){
    //bit must be between 0 and 7 since registers
    //are 8 bits
    assert(bit > -1 && bit < 8);
    //only setting one bit so val must be 0 or 1
    assert (val == 0 || val == 1);
    c->regs[name] = (c->regs[name] & ~(1 << bit)) | 
                    (val << bit);
}

int8_t getRegBit(CPU *c, REG name, int8_t bit){
    //bit must be between 0 and 7 since registers
    //are 8 bits
    assert(bit > -1 && bit < 8);
    return (c->regs[name] & (1 << bit)) > 0 ? 1 : 0;
}

/* CPU initializer */
CPU * getCPU(){
    CPU *c = malloc(sizeof(CPU));
    c-> PC = 0;
    //initialize 8 bit registers
    int NUM_REG = 5;
    int8_t *r = calloc(NUM_REG,sizeof(int8_t));
    c-> regs = r;
    //initialize address space
    int ADDR_SPACE_SIZE = 65536;
    int8_t *a= calloc(ADDR_SPACE_SIZE,sizeof(int8_t));
    c-> addressSpace = a;
    //set bit 5 of status register to 1
    //to match specifications
    setRegBit(c, STATUS, 5, 1);
    return c;
}

void resetCPU(CPU *c){
    //resets CPU to initial state as if it has just been initialized
    int NUM_REG = 5;
    memset(c->regs, 0, NUM_REG*sizeof(int8_t));
    int ADDR_SPACE_SIZE = 65536;
    memset(c->addressSpace, 0, ADDR_SPACE_SIZE*sizeof(int8_t));
    c->PC = 0;
    setRegBit(c, STATUS, 5, 1);
}

void freeCPU(CPU *c){
    free(c->regs);
    free(c->addressSpace);
    free(c);
}

/* prints state of CPU registers */
void print(CPU *c){
    printf("PC: ");
    printf("%d\n", c->PC);
    printf("SVUBDIZC\n");
    printf("%s\n",getStatus(c));
    printf("STACK REG: ");
    printf("%d\n", c->regs[STACK]);
    printf("ACCUM REG: ");
    printf("%d\n", c->regs[ACCUM]);
    printf("IND_X REG: ");
    printf("%d\n", c->regs[IND_X]);
    printf("IND_Y REG: ");
    printf("%d\n", c->regs[IND_Y]);
}

char *getStatus(CPU *c){
    return byte_to_binary(c->regs[STATUS]);
}

char *byte_to_binary(int x){
    char *b = malloc(sizeof(char)*9);
    b[0] = '\0';
    int z;
    for (z = 128; z > 0; z >>= 1)
    {
        strcat(b, ((x & z) == z) ? "1" : "0");
    }
    return b;
}

/* set byte value in one of the eight bit 
 * registers to val */
void setRegByte(CPU *c, REG name, int8_t val){
    c->regs[name] = val;
}

int8_t getRegByte(CPU *c, REG name){
    return c->regs[name];
}

void setFlag(CPU *c, FLAG name, int8_t val){
    //bit 5 of the status register is not to be set
    //and should always be 1
    assert(name != NOT_USED_FLAG);
    setRegBit(c, STATUS, name, val);
}

int8_t getFlag(CPU *c, FLAG name){
    //bit 5 of the status register is not to be set
    //and should always be 1
    assert(name != NOT_USED_FLAG);
    return getRegBit(c, STATUS, name);
}

int8_t read(CPU *c, uint16_t addr){
    //placeholder code 
    //TODO: replace this w/ actual implementation
    int8_t placeholder = c->addressSpace[addr];
    return placeholder;
}

void write(CPU *c, uint16_t addr, int8_t val){
    c->addressSpace[addr] = val;
}

/* FLAG REG OPERATIONS */

void setCarry(CPU *c, int16_t val){
    //sets carry flag if
    //val > 0b11111111, largest
    //eight bit val
    int8_t carry = val > 0xFF ? 1 : 0;
    setFlag(c,C,carry);
}

void setCarryBCD(CPU *c, int16_t val){
    //sets carry flag if
    //val > 0x99, for addition in BCD mode
    int8_t carry = val > 0x99 ? 1 : 0;
    setFlag(c,C,carry);
}

void setOverflow(CPU *c, int8_t a, int8_t b, int8_t val){
    //sets overflow if overflow in twos complement 
    //occurred when adding a and b to get val
    //this bit twiddling from:
    //http://nesdev.com/6502.txt
    int8_t overflow = !((a ^ b) & 0x80) && ((a ^ val) & 0x80);
    //overflow = signs of operands are the same AND
    //          sign of result not equal to sign of operands
    setFlag(c,V,overflow);
}

void setOverflowSubtract(CPU *c, int8_t a, int8_t b, int8_t val){
    //sets overflow if overflow in twos complement 
    //occurred when subtracting b from a to get val
    //this bit twiddling from:
    //http://nesdev.com/6502.txt
    int8_t overflow = ((a ^ b) & 0x80) && ((a ^ val) & 0x80);
    //overflow = signs of operands are the same AND
    //          sign of result not equal to sign of operands
    setFlag(c,V,overflow);
}

void setSign(CPU *c, int8_t val){
    //sets sign flag equal to sign
    //of bit 7 of val
    int8_t sign = val & 0x80? 1 : 0;
    setFlag(c,S,sign);
}

void setZero(CPU *c, int8_t val){
    //sets zero flag to 0 if val == 0
    //and zero flag to 1 otherwise
    int8_t isZero = val? 0 : 1 ;
    setFlag(c,Z,isZero);
}

OP_CODE_INFO * getOP_CODE_INFO(int8_t operand, uint16_t address, MODE mode){
    OP_CODE_INFO *o = malloc(sizeof(OP_CODE_INFO));
    o->operand = operand;
    o->address = address;
    o->mode = mode;
    return o;
}

void freeOP_CODE_INFO(OP_CODE_INFO *o){
    free(o);
}

/* STACK OPERATIONS HERE */
void PUSH( __attribute__ ((unused)) CPU *c, 
        __attribute__ ((unused)) OP_CODE_INFO *o, 
        __attribute__ ((unused)) int8_t operand){
    //TODO: implement pushing operand onto stack
}

/* OP CODE IMPLEMENTATIONS HERE */

//Add with carry
void ADC(CPU *c, OP_CODE_INFO *o){
    int8_t carry = getFlag(c,C);
    int8_t accum = getRegByte(c,ACCUM);
    int8_t operand = o->operand;
    int16_t sum = (0x00FF&carry) + (0x00FF&accum) + (0x00FF&operand);
    int8_t sumByte = sum & 0x00FF;
    setZero(c,sumByte);
    if(getFlag(c,D)){ //in decimal mode
        //if lower 4 bits of operands plus
        //the carry in are larger than 9,
        //then we need to apply conversions
        //to remain in binary coded decimal format.
        if((accum & 0xF) + (operand & 0xF)
            + carry > 9){
            sum += 6;
        }
        setSign(c,sumByte);
        setOverflow(c,accum,operand,sumByte);
        //if the higher bits aren't in
        //BCD format we need to add 96 to convert.
        //Black magic from http://nesdev.com/6502.txt
        sum += sum > 0x99 ? 96 : 0;
        setCarryBCD(c, sum);
    } else {
        setSign(c,sumByte);
        setOverflow(c,accum,operand,sumByte);
        setCarry(c,sum);
    }
    setRegByte(c,ACCUM,sum&0xFF);
}

void AND(CPU *c, OP_CODE_INFO *o){
    int8_t accum = getRegByte(c,ACCUM);
    int8_t operand = o->operand;
    int8_t res = accum & operand;
    setSign(c,res);
    setZero(c,res);
    setRegByte(c,ACCUM,res);
}

//Arithmetic shift left
void ASL(CPU *c, OP_CODE_INFO *o){
    int8_t operand = o->operand;
    int16_t res = (0x00FF&operand) << 1;
    int8_t resByte = res & 0x00FF;
    setCarry(c,res);
    setSign(c,resByte);
    setZero(c,resByte);
    if(o->mode == Accumulator){
        setRegByte(c,ACCUM,res);
    } else {
        write(c,o->address,res);
    }
}

//Branch if carry clear
void BCC(CPU *c, OP_CODE_INFO *o){
    if(!getFlag(c,C)){
        c->PC = o->address;
        //TODO: cpu add branch cycles here
    }
}

//Branch if carry set
void BCS(CPU *c, OP_CODE_INFO *o){
    if(getFlag(c,C)){
        c->PC = o->address;
        //TODO: cpu add branch cycles here
    }
}

//Branch if equals
void BEQ(CPU *c, OP_CODE_INFO *o){
    if(getFlag(c,Z)){
        c->PC = o->address;
        //TODO: cpu add branch cycles here
    }
}

// Test bits in memory with accumulator
void BIT(CPU *c, OP_CODE_INFO *o){

    int8_t src = o->operand;
    int8_t accum = getRegByte(c, ACCUM);

    setFlag(c, V, (src & 0x40)); // get 6th bit of src
    setFlag(c, S, src); // get 7th bit of src
    setFlag(c, Z, (src & accum));
}

// Branch if result minus
void BMI(CPU *c, OP_CODE_INFO *o){
    if (getFlag(c, S)) {
        c->PC = o->address;
        //TODO: cpu add branch cycles here
    }
}

// Branch if not equals
void BNE(CPU *c, OP_CODE_INFO *o){
    if(!(getFlag(c,Z))){
        c->PC = o->address;
        //TODO: cpu add branch cycles here
    }
}

// Branch if result plus
void BPL(CPU *c, OP_CODE_INFO *o){
    if(!(getFlag(c,S))){
        c->PC = o->address;
        //TODO: cpu add branch cycles here
    }
}

// TODO BRK
// Force Break
void BRK(CPU *c, OP_CODE_INFO *o){
    (c->PC)++; // this could be between 0 and 3.
    PUSH(c, o, (c->PC >> 8) & 0xff); // Push return address onto the stack.
    PUSH(c, o, c->PC & 0xff); // (push Program Counter AND 0xFF)
    setFlag(c, B, 1);
    PUSH(c, o, getRegByte(c, STATUS)); // (push status register onto top of stack)
    setFlag(c, D, 1);
    // PC = (LOAD(0xFFFE) | (LOAD(0xFFFF) << 8)); change PC
}

// Branch if overflow clear
void BVC(CPU *c, OP_CODE_INFO *o){
    if (!(getFlag(c, V))){
        c->PC = o->address;
        //TODO: add clk cycles here
    }
}

//Branch on overflow set
void BVS(CPU *c, OP_CODE_INFO *o){
    if (getFlag(c, V)){
        c->PC = o->address;
        //TODO: add clk cycles here
    }
}

//Clear carry flag
void CLC(CPU *c, __attribute__ ((unused)) OP_CODE_INFO *o){
    setFlag(c, C, 0);
}

//Clear decimal mode
void CLD(CPU *c, __attribute__ ((unused)) OP_CODE_INFO *o){
    setFlag(c, D, 0);
}

//Clear interrupt disable bit
void CLI(CPU *c, __attribute__ ((unused)) OP_CODE_INFO *o){
    setFlag(c, I, 0);
}

//Clear overflow flag
void CLV(CPU *c, __attribute__ ((unused)) OP_CODE_INFO *o){
    setFlag(c, V, 0);
}

//Compare memory and accumulator
void CMP(CPU *c, OP_CODE_INFO *o){
    int8_t src = getRegByte(c, ACCUM) - o->address;
    setFlag(c, C, src < 0x100);
    setFlag(c, S, (src & 0x40));    // get 7th bit of src
    setFlag(c, Z, (src &= 0xff));
}

//Compare memory and index x
void CPX(CPU *c, OP_CODE_INFO *o){
    int8_t src = getRegByte(c, IND_X) - o->address;
    setFlag(c, C, src < 0x100);
    setFlag(c, S, (src & 0x40));    // get 7th bit of src
    setFlag(c, Z, (src &= 0xff));
}

//Compare memory and index y
void CPY(CPU *c, OP_CODE_INFO *o){
    int8_t src = getRegByte(c, IND_Y) - o->address;
    setFlag(c, C, src < 0x100);
    setFlag(c, S, (src & 0x40));    // get 7th bit of src
    setFlag(c, Z, (src &= 0xff));
}

//Jump PC to 16 bit operand
void JMP(CPU *c, OP_CODE_INFO *o){
    c->PC = 0xFFFF&(o->address);
}

//Jump to subroutine
void JSR(CPU *c, OP_CODE_INFO *o){
    //decrement PC because PC will be
    //incremented by one when it jumps back
    c->PC--;
    //STACK holds eight bit values
    //so we push the 16 bit address
    //onto the stack in two parts
    PUSH(c, o, ((c->PC)>>8) & 0xFF);
    PUSH(c, o, c->PC & 0xFF);
    c->PC = o->operand;
}

//Load value into accumulator
void LDA(CPU *c, OP_CODE_INFO *o){
    setSign(c, o->operand);
    setZero(c, o->operand);
    setRegByte(c, ACCUM, o->operand);
}

//Load value into x reg
void LDX(CPU *c, OP_CODE_INFO *o){
    setSign(c, o->operand);
    setZero(c, o->operand);
    setRegByte(c, IND_X, o->operand);
}

//Load value into y reg
void LDY(CPU *c, OP_CODE_INFO *o){
    setSign(c, o->operand);
    setZero(c, o->operand);
    setRegByte(c, IND_Y, o->operand);
}

//Logical shift right
void LSR(CPU *c, OP_CODE_INFO *o){
    //shift rightmost bit into carry
    setFlag(c, C, o->operand & 0x01);
    int8_t shifted = o->operand >> 1;
    setSign(c, shifted);
    setZero(c, shifted);
    if(o->mode == Accumulator){
        setRegByte(c, ACCUM, shifted);
    } else {
        write(c, o->address, shifted);
    }
}

// No operation
void NOP(CPU *c, OP_CODE_INFO *o){}

// OR memory with accumulator
void ORA(CPU *c, OP_CODE_INFO *o){
    int8_t src = o->operand | getRegByte(c, ACCUM);
    setFlag(c, Z, src);
    setFlag(c, S, src);
    setRegByte(c, ACCUM, src);
}

// Push accumulator onto stack
void PHA(CPU *c, OP_CODE_INFO *o){
    PUSH(c, o, getRegByte(c, ACCUM));
}

// Push status register onto stack
void PHP(CPU *c, OP_CODE_INFO *o){
    PUSH(c, o, getRegByte(c, STATUS));
}

// Pull accumulator from stack
void PLA(CPU *c, OP_CODE_INFO *o){
    // setRegByte(c, ACCUM, PULL(c));
}

// Pull status register from stack
void PLP(CPU *c, OP_CODE_INFO *o){
    // setRegByte(c, STATUS, PULL(c));
}

// Subtract operand from accumulator with borrow
void SBC(CPU *c, OP_CODE_INFO *o){
    int8_t carry = getFlag(c,C);
    int8_t accum = getRegByte(c,ACCUM);
    int8_t operand = o->operand;
    int16_t diff = (0x00FF&accum) - (0x00FF&operand) - (0x00FF&carry);
    int8_t diffByte = diff & 0x00FF;
    setSign(c,diffByte);
    setZero(c,diffByte);
    setOverflowSubtract(c,accum,operand,diffByte);
    if(getFlag(c,D)){ //in decimal mode
        if((accum & 0xF) - carry < (operand & 0xF)){
            diff -= 6;
        }
        if(diff > 0x99){
            diff -= 0x60;
        }
    }
    setFlag(c,C, diff < 0x100 ? 1 : 0);
    setRegByte(c,ACCUM,diff&0xFF);
}

// Set carry flag to 1
void SEC(CPU *c, OP_CODE_INFO *o){
    setFlag(c, C, 1);
}

// Store accumulator reg into memory
void STA(CPU *c, OP_CODE_INFO *o){
    write(c, o->address, getRegByte(c,ACCUM));
}

// Store X reg into memory
void STX(CPU *c, OP_CODE_INFO *o){
    write(c, o->address, getRegByte(c,IND_X));
}

// Transfer accumulator to Y reg
void TAY(CPU *c, OP_CODE_INFO *o){
    int8_t accumVal = getRegByte(c,ACCUM);
    setRegByte(c,IND_Y,accumVal);
}

// Transfer Y to accumulator
void TYA(CPU *c, OP_CODE_INFO *o){
    int8_t accumVal = getRegByte(c,IND_Y);
    setRegByte(c,ACCUM,accumVal);
}

/*
int main ()
{
    CPU *c = getCPU();
    print(c);
    return 0;
}
*/
