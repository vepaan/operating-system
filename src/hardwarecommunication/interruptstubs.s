.set IRQ_BASE, 0x20

.section .text

.extern _ZN4myos21hardwarecommunication16InterruptManager15handleInterruptEhj # from nm interrupts.o

.global _ZN4myos21hardwarecommunication16InterruptManager22IgnoreInterruptRequestEv

.macro HandleException num
.global _ZN4myos21hardwarecommunication16InterruptManager16HandleException\num\()Ev
_ZN4myos21hardwarecommunication16InterruptManager16HandleException\num\()Ev:
  movb $\num, (interruptnumber)
  jmp int_bottom
.endm

.macro HandleInterruptRequest num
.global _ZN4myos21hardwarecommunication16InterruptManager26HandleInterruptRequest\num\()Ev
_ZN4myos21hardwarecommunication16InterruptManager26HandleInterruptRequest\num\()Ev:
  movb $\num + IRQ_BASE, (interruptnumber)
  pushl $0 # to handle error in CPUState
  jmp int_bottom
.endm

HandleInterruptRequest 0x00
HandleInterruptRequest 0x01
HandleInterruptRequest 0x0C

int_bottom:

  # save register
  # pusha
  # pushl %ds
  # pushl %es
  # pushl %fs
  # pushl %gs

  pushl %ebp
  pushl %edi
  pushl %esi

  pushl %edx
  pushl %ecx
  pushl %ebx
  pushl %eax

  # call C++ handler
  pushl %esp
  push (interruptnumber)
  call _ZN4myos21hardwarecommunication16InterruptManager15handleInterruptEhj
  # addl $5, %esp
  movl %eax, %esp # switch the stack

  # restore registers
  # popl %gs
  # popl %fs
  # popl %es 
  # popl %ds
  # popa
  popl %eax
  popl %ebx
  popl %ecx
  popl %edx

  popl %esi
  popl %edi
  popl %ebp

  add $4, %esp

_ZN4myos21hardwarecommunication16InterruptManager22IgnoreInterruptRequestEv:

  iret

.data
  interruptnumber: .byte 0
