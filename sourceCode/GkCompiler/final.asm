.386
.model flat, stdcall
include include\msvcrt.inc
includelib lib\msvcrt.lib
.data
       szFormat  db    '%c',0
.code
charTest proc C 
push ebp
mov ebp,esp
sub esp,32
mov eax,0
shl eax,2
add eax,12
mov ebx,ebp
sub ebx,eax
mov eax,65
mov dword ptr ss:[ebx],eax
mov eax,1
shl eax,2
add eax,12
mov ebx,ebp
sub ebx,eax
mov eax,66
mov dword ptr ss:[ebx],eax
mov eax,2
shl eax,2
add eax,12
mov ebx,ebp
sub ebx,eax
mov eax,67
mov dword ptr ss:[ebx],eax
mov eax,3
shl eax,2
add eax,12
mov ebx,ebp
sub ebx,eax
mov eax,68
mov dword ptr ss:[ebx],eax
mov eax,3
shl eax,2
add eax,12
mov ebx,ebp
sub ebx,eax
mov eax,dword ptr ss:[ebx]
mov dword ptr ss:[ebp-28],eax
push dword ptr ss:[ebp-28]
lea eax,szFormat
push eax
call crt_printf
mov eax,2
shl eax,2
add eax,12
mov ebx,ebp
sub ebx,eax
mov eax,dword ptr ss:[ebx]
mov dword ptr ss:[ebp-28],eax
push dword ptr ss:[ebp-28]
lea eax,szFormat
push eax
call crt_printf
mov eax,dword ptr ss:[ebp-28]
add esp,32
mov esp,ebp
pop ebp
ret
charTest endp
start:
push ebp
mov ebp,esp
sub esp,12
call charTest
add esp,12
mov esp,ebp
pop ebp
ret
end start
charTest proc C 
push ebp
mov ebp,esp
sub esp,32
mov eax,0
shl eax,2
add eax,12
mov ebx,ebp
sub ebx,eax
mov eax,65
mov dword ptr ss:[ebx],eax
mov eax,1
shl eax,2
add eax,12
mov ebx,ebp
sub ebx,eax
mov eax,66
mov dword ptr ss:[ebx],eax
mov eax,2
shl eax,2
add eax,12
mov ebx,ebp
sub ebx,eax
mov eax,67
mov dword ptr ss:[ebx],eax
mov eax,3
shl eax,2
add eax,12
mov ebx,ebp
sub ebx,eax
mov eax,68
mov dword ptr ss:[ebx],eax
mov eax,3
shl eax,2
add eax,12
mov ebx,ebp
sub ebx,eax
mov eax,dword ptr ss:[ebx]
mov dword ptr ss:[ebp-28],eax
push dword ptr ss:[ebp-28]
lea eax,szFormat
push eax
call crt_printf
mov eax,2
shl eax,2
add eax,12
mov ebx,ebp
sub ebx,eax
mov eax,dword ptr ss:[ebx]
mov dword ptr ss:[ebp-28],eax
push dword ptr ss:[ebp-28]
lea eax,szFormat
push eax
call crt_printf
mov eax,dword ptr ss:[ebp-28]
add esp,32
mov esp,ebp
pop ebp
ret
charTest endp
start:
push ebp
mov ebp,esp
sub esp,12
call charTest
add esp,12
mov esp,ebp
pop ebp
ret
end start
