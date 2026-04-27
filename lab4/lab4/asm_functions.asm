.386
.MODEL FLAT, C

.CODE

; Прототип: int CountInInterval(int* arr, int size, int num, int avg)
CountInInterval PROC C arr:PTR DWORD, size:DWORD, num:DWORD, avg:DWORD
    
    push ebp
    mov ebp, esp
    
    ; Получаем параметры
    mov esi, arr        ; указатель на массив
    mov ecx, size       ; количество элементов
    mov eax, num        ; заданное число
    mov ebx, avg        ; среднее арифметическое
    
    ; Проверка на пустой массив
    cmp ecx, 0
    je empty_array
    
    ; Определяем lower и upper
    cmp eax, ebx
    jge num_greater
    
    ; num < avg
    mov edx, eax        ; lower = num
    mov edi, ebx        ; upper = avg
    jmp bounds_ready
    
num_greater:
    ; num >= avg
    mov edx, ebx        ; lower = avg
    mov edi, eax        ; upper = num
    
bounds_ready:
    ; Подсчет элементов в интервале (lower, upper) исключая границы
    xor eax, eax        ; eax = счетчик (результат)
    xor ebx, ebx        ; ebx = индекс
    
count_loop:
    cmp ebx, ecx
    jge count_done
    
    ; Получаем текущий элемент
    mov edi, [esi + ebx * 4]
    
    ; Сравниваем с lower
    cmp edi, edx
    jle skip_element
    
    ; Сравниваем с upper
    cmp edi, [ebp + 16]  ; сравниваем с num
    jge skip_element_upper
    
    jmp count_element
    
skip_element_upper:
    cmp edi, [ebp + 20]  ; сравниваем с avg
    jge skip_element
    
count_element:
    inc eax
    
skip_element:
    inc ebx
    jmp count_loop
    
count_done:
    pop ebp
    ret
    
empty_array:
    xor eax, eax
    pop ebp
    ret
    
CountInInterval ENDP

END