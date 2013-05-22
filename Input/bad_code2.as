    mov/0 eax,var1
    cmp/0/1/1 eax,var2
    jne/1 skip
    pop/0 eax
    push jmpAddress
    skip:
    ret
jumpIfEqual: ENDP

....

push OFFSET jumpToHere
mov eax, 5
push eax
push/1 eax
call jumpIfEqual
