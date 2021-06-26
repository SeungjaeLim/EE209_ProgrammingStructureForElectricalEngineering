### 20190545 / Seungjae Lim / Assignment 4
### --------------------------------------------------------------------
### mydc.s
###
### Desk Calculator (dc)
### --------------------------------------------------------------------

	.equ   ARRAYSIZE, 20
	.equ   EOF, -1
        .equ   THREEPARAM, 12
        .equ   TWOPARAM, 8
        .equ   ONEPARAM, 4
        .equ   ZERO, 0
        .equ   SIGNMASK, 31
	
.section ".rodata"

## string for format
scanfFormat:
	.asciz "%s"

printfFormat:
        .asciz "%d\n"

## string for error handling
emptyError:
        .asciz "dc: stack empty\n"

devideError:
        .asciz "dc: Zero Division Error\n"

## string for debugging
testprintfFormat:
        .asciz "%c\n"

testtxt:
        .asciz "testing...\n"
### --------------------------------------------------------------------

        .section ".data"

### --------------------------------------------------------------------

        .section ".bss"
buffer:
        .skip  ARRAYSIZE

### --------------------------------------------------------------------

	.section ".text"

	## -------------------------------------------------------------
	## int main(void)
	## Runs desk calculator program.  Returns 0.
	## -------------------------------------------------------------

	.globl  main
	.type   main,@function

main:

	pushl   %ebp
	movl    %esp, %ebp

input:

	## dc number stack initialized. %esp = %ebp
	
	## scanf("%s", buffer)
	pushl	$buffer
	pushl	$scanfFormat
	call    scanf
	addl    $TWOPARAM, %esp

	## check if user input EOF
	cmp	$EOF, %eax
	je	quit
        
        movl    $buffer, %esi
        ## clear %ebx for use %bl
        xor     %ebx, %ebx
        ## %bl <- buffer[0] 
        movb    (%esi), %bl

        ## check is number digit
        ## isdigit(buffer[0])
        pushl   %ebx
        call    isdigit
        add     $ONEPARAM, %esp
        andl    %eax, %eax
        ## when buffer[0] is digit
        jne     digit

## when buffer[0] is not digit -> command or garbage
command:
        ## _ -> minus digit
        cmpb    $'_', %bl
        je      minus

        ## + -> add a,b
        cmpb    $'+', %bl
        je      add
        
        ## - -> sub a,b
        cmpb    $'-', %bl
        je      sub
        
        ## * -> mult a to b
        cmpb    $'*', %bl
        je      mult
        
        ## / -> div a to b
        cmpb    $'/', %bl
        je      div
        
        ## | -> absolute sum min(a,b) to max(a,b)
        cmpb    $'|', %bl
        je      vertical_var
        
        ## p -> print top of stack
        cmpb    $'p', %bl
        je      print
        
        ## q -> quit
        cmpb    $'q', %bl
        je      quit
        
        ## f -> flush stack, not clear
        cmpb    $'f', %bl
        je      flush
        
        ## c -> clear stack
        cmpb    $'c', %bl
        je      clear
        
        ## d -> duplicate top of stack
        cmpb    $'d', %bl
        je      duplicate
        
        ## r -> reverse first 2 number
        cmpb    $'r', %bl
        je      reverse

        ## garbage value -> skip
        jmp     input

## handle digit string to push integer
digit:
        ## num = atoi(buffer)
        pushl   $buffer
        call    atoi
        addl    $ONEPARAM, %esp

        ## stack.push(num)
        pushl   %eax
        jmp     input

## error handle when no first parameter in stack
poperror1:
        ## print error message
        pushl   $emptyError
        call    printf
        add     $ONEPARAM, %esp
        ## skip command
        jmp     input

## error handle when no second parameter in stack
poperror2:
        ## push back first element in stack
        pushl   %ebx
        ## print error message
        pushl   $emptyError
        call    printf
        add     $ONEPARAM, %esp
        ## skip command
        jmp     input

## error handle when devide with zero
devidezero:
        ## push back first element in stack
        pushl   %ebx
        ## print error message
        pushl   $devideError
        call    printf
        add     $ONEPARAM, %esp
        ## skip command
        jmp     input        

## buffer[0] == '_'
## make digit negative
minus:
        ## atoi(number+1), next of '_'
        movl    $buffer, %esi
        incl    %esi
        pushl   %esi
        call    atoi
        add     $ONEPARAM, %esp
        ## give minus sign
        negl    %eax
        pushl   %eax
        jmp     input

## buffer[0] == '+'
## add top of two element
add:
        ## check is there first parameter
        cmpl    %esp, %ebp
        ## stack empty error occur
        jle     poperror1
        popl    %ebx
        ## check is there second parameter
        cmpl    %esp, %ebp
        ## stack empty error occur
        jle     poperror2
        popl    %eax

        ## a = a + b
        addl    %ebx, %eax
        pushl   %eax
        jmp     input

## buffer[0] == '-'
## subtract top of two element
sub:
        ## check is there first parameter
        cmpl    %esp, %ebp
        ## stack empty error occur
        jle     poperror1
        popl    %ebx
        ## check is there second parameter
        cmpl    %esp, %ebp
        ## stack empty error occur
        jle     poperror2
        popl    %eax

        ## a = a - b
        subl    %ebx, %eax
        pushl   %eax
        jmp     input

## buffer[0] == '*'
## multiply top of two element
mult:
        ## check is there first parameter
        cmpl    %esp, %ebp
        ## stack empty error occur
        jle     poperror1
        popl    %ebx
        ## check is there second parameter
        cmpl    %esp, %ebp
        ## stack empty error occur
        jle     poperror2
        popl    %eax

        ## a = a * b
        mull    %ebx
        pushl   %eax
        jmp     input

## buffer[0] == '/'
## devide top of two element
div:
        ## check is there first parameter
        cmpl    %esp, %ebp
        ## stack empty error occur
        jle     poperror1
        popl    %ebx

        ## check devide with Zero
        cmpl    $ZERO, %ebx
        je      devidezero

        ## check is there second parameter
        cmpl    %esp, %ebp
        ## stack empty error occur
        jle     poperror2
        popl    %eax

        ## initialize %edx 0 or -1 for divide
        movl    %eax, %ecx
        sarl    $SIGNMASK, %ecx
        movl    %ecx, %edx

        ## a = a / b
        idivl   %ebx
        pushl   %eax
        jmp     input

## buffer[0] == '|'
## call abssum() function to get abs sum min(a,b) to max(a,b)
vertical_var:
        ## check is there first parameter
        cmpl    %esp, %ebp
        ## stack empty error occur
        jle     poperror1
        popl    %ebx
        ## check is there second parameter
        cmpl    %esp, %ebp
        ## stack empty error occur
        jle     poperror2
        popl    %eax

        ## call abssum function
        pushl   %eax
        pushl   %ebx
        call    abssum
        addl    $TWOPARAM, %esp
        pushl   %eax
        jmp     input

## buffer[0] == 'p'
## print top of stack
print:
        ## check is there first parameter
        cmpl    %esp, %ebp
        ## stack empty error occur
        jle     poperror1

        ## printf("%d\n", stack.top())
        pushl   $printfFormat
        call    printf
        add     $ONEPARAM, %esp
        jmp     input

## buffer[0] == 'q' or EOF
## quit dc program
quit:	
	## return 0
	movl    $0, %eax
	movl    %ebp, %esp
	popl    %ebp
	ret

## buffer[0] == 'f'
## print stack LIFO order
flush:
        ## initialize %esi by %esp for traverse stack
        movl    %esp, %esi
        jmp     flushloop

## traverse stack for print
flushloop:
        ## when reach to end of stack
        cmpl    %esi, %ebp
        ## finish flushing
        jle     input

        ## print current part of stack
        pushl   (%esi)
        pushl   $printfFormat
        call    printf

        ## clean stack for print
        addl    $TWOPARAM, %esp

        ## traverse next element in stack
        addl    $ONEPARAM, %esi
        jmp     flushloop

## buffer[0] == 'c'
## clear stack
clear:
        ## when reach to end of stack
        cmpl    %esp, %ebp
        ## finish cleaning
        jle     input

        ## pop stack until stack is empty
        popl    %edx
        jmp     clear

## buffer[0] == 'd'
## duplicate top of two element
duplicate:
        ## check is there first parameter
        cmpl    %esp, %ebp
        ## stack empty error occur
        jle     poperror1

        ## get value top of stack 
        popl    %eax
        ## push top of stack twice
        pushl   %eax
        pushl   %eax
        jmp     input

## buffer[0] == 'r'
## reverse top of two element 
reverse:
        ## check is there first parameter
        cmpl    %esp, %ebp
        ## stack empty error occur
        jle     poperror1
        popl    %ebx
        ## check is there second parameter
        cmpl    %esp, %ebp
        ## stack empty error occur
        jle     poperror2
        popl    %eax

        ## swap order of first 2 element
        pushl   %ebx
        pushl   %eax
        jmp     input
        
## calculate absolute sum between two parameter
abssum:
        pushl   %ebp
        movl    %esp, %ebp

        ## get parameter
        movl    TWOPARAM(%esp), %eax
        movl    THREEPARAM(%esp), %ebx
        
        ## make min in %eax, max in %ebx
        cmpl    %ebx, %eax
        jle     aismin

        ## when %eax >= %ebx -> swap
        movl    %eax, %edx
        movl    %ebx, %eax
        movl    %edx, %ebx

        ## initialize i to %ecx, sum to %edx
        movl    %eax, %ecx
        movl    $ZERO, %edx
        jmp     sumloop

## for loop of absolute sum
sumloop:
        ## keep in loop only i <= max(a,b)
        cmpl    %ebx, %ecx
        jg      endloop

        ## save i to stack
        pushl   %ecx

        ## calculate abs(i)
        movl    %ecx, %eax
 
        ## abs(x) = x ^ (x>>31) - (x>>31)
        sarl    $SIGNMASK, %eax
        xorl    %eax, %ecx
        subl    %eax, %ecx

        ## sum += abs(i)
        addl    %ecx, %edx

        ## load i from stack
        popl    %ecx
        ## i++
        incl    %ecx
        jmp     sumloop

## end of abssum(), return
endloop:
        movl    %edx, %eax
        movl    %ebp, %esp
        popl    %ebp
        ret

## already %eax < %ebx
aismin:
        movl    %eax, %ecx
        movl    $ZERO, %edx
        jmp     sumloop
