.386
.model flat, stdcall
option casemap:none

; Подключение библиотек
includelib kernel32.lib
includelib msvcrt.lib

; Прототипы функций
ExitProcess proto :DWORD
printf proto C :ptr byte, :vararg
scanf proto C :ptr byte, :vararg
SetConsoleOutputCP proto :DWORD
SetConsoleCP proto :DWORD

; Сегмент данных
.data
    fmt_input   db "Введите значения a, b, c, d: ", 0
    fmt_scan    db "%d %d %d %d", 0
    fmt_result  db "Результат = %d", 10, 0
    msg_zero    db "Ошибка: деление на ноль!", 10, 0
    msg_over    db "Ошибка: переполнение!", 10, 0
    
    ; Переменные
    a           sdword ?
    b           sdword ?
    c           sdword ?
    d           sdword ?
    result      sdword 0

; Сегмент кода
.code
start:
    ; Установка кодировки UTF-8 для корректного вывода русских букв
    push    65001
    call    SetConsoleOutputCP
    push    65001
    call    SetConsoleCP
    
    ; Вывод приглашения
    push    offset fmt_input
    call    printf
    add     esp, 4
    
    ; Ввод a, b, c, d
    push    offset d
    push    offset c
    push    offset b
    push    offset a
    push    offset fmt_scan
    call    scanf
    add     esp, 20
    
   
    ; Вычисление (4*b/c - d) / (12*c + a - b)
    
    
    ; --- Вычисляем числитель: 4*b/c - d ---
    
    ; eax = 4*b с проверкой переполнения
    mov     eax, b
    mov     ecx, 4
    imul    eax, ecx            ; eax = 4*b
    jo      error_of            ; переполнение при умножении
    
    ; Проверка деления на ноль (c == 0)
    mov     ecx, c
    test    ecx, ecx
    jz      error_zf            ; деление на ноль
    
    ; eax = (4*b) / c
    cdq                         ; расширяем знак eax в edx:eax
    idiv    ecx                 ; eax = 4*b / c, edx = остаток
    jo      error_of            ; переполнение при делении (маловероятно, но на всякий случай)
    
    ; eax = 4*b/c - d
    sub     eax, d
    jo      error_of            ; переполнение при вычитании
    
    push    eax                 ; сохраняем числитель на стеке
    
    ; --- Вычисляем знаменатель: 12*c + a - b ---
    
    mov     eax, c
    mov     ecx, 12
    imul    eax, ecx            ; eax = 12*c
    jo      error_of
    
    add     eax, a              ; eax = 12*c + a
    jo      error_of
    
    sub     eax, b              ; eax = 12*c + a - b
    jo      error_of
    
    mov     ebx, eax            ; ebx = знаменатель
    
    ; Проверка деления на ноль
    test    ebx, ebx
    jz      error_zf
    
    ; --- Деление числителя на знаменатель ---
    
    pop     eax                 ; восстанавливаем числитель
    cdq                         ; расширяем знак
    idiv    ebx                 ; eax = числитель / знаменатель
    jo      error_of
    
    mov     result, eax
    jmp     print_result
    
error_zf:
    push    offset msg_zero
    call    printf
    add     esp, 4
    jmp     exit_prog
    
error_of:
    ; Очистка стека если там остался сохранённый числитель
    add     esp, 4              ; убираем сохранённое значение со стека
    push    offset msg_over
    call    printf
    add     esp, 4
    jmp     exit_prog
    
print_result:
    push    result
    push    offset fmt_result
    call    printf
    add     esp, 8
    
exit_prog:
    push    0
    call    ExitProcess

end start