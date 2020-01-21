 # labwork.S
# Written 2015-2017 by F Lundevall
# Skeleton file for IS1200/IS1500 lab 1.
# The macros PUSH and POP are in the public domain.

.macro	PUSH reg
addi	$sp,$sp,-4
sw	\reg,0($sp)
.endm

.macro	POP reg
lw	\reg,0($sp)
addi	$sp,$sp,4
.endm

 .data
 .global delay
 .align 2
 .text

  delay:
  PUSH	$ra
  li	$t2, 0 			        # int i = 0
  li 	$t3, 4800000		    # konstant 4800000

 while:
    ble	$a0, 0, done 	        # branch till done om ms <= 0
    nop
    sub 	$a0, $a0, 1	        # ms = ms-1

    for:
      beq 	$t2, $t3, while		# i < 4711
      nop
      addi 	$t2, $t2, 1	    	# i = i+1
      j for			            # tillbaka till for
      nop

 done:
  POP $ra
  jr $ra
  nop

