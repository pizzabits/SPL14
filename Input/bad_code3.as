print_int:          ; Breaks number in ax register to print it
    mov cx, 10
    mov bx, 0
    break_num_to_pics:
        cmp ax, 0
        je print_all_pics
        div cx
        push dx
        inc bx
        jmp break_num_to_pics
    print_all_pics:
        cmp bx, 0       ; tests if bx is already null
        je f_end
        pop ax
        add ax, 30h
        call print_char
        dec bx
        jmp print_all_pics

print_char:             ; Function to print single character to      screen
        mov ah, 0eh     ; Prepare registers to print the character
        int 10h         ; Call BIOS interrupt
        ret

f_end:              ; Return back upon function completion
    ret
