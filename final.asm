.386
.model flat, stdcall
include include\msvcrt.inc
includelib lib\msvcrt.lib
.data
       szFormat  db    '%c',0
.code
sort proc C 
push ebp
mov ebp,esp
sub esp,72
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
mov eax,67
mov dword ptr ss:[ebx],eax
mov eax,2
shl eax,2
add eax,12
mov ebx,ebp
sub ebx,eax
mov eax,73
mov dword ptr ss:[ebx],eax
mov eax,3
shl eax,2
add eax,12
mov ebx,ebp
sub ebx,eax
mov eax,78
mov dword ptr ss:[ebx],eax
mov eax,4
shl eax,2
add eax,12
mov ebx,ebp
sub ebx,eax
mov eax,66
mov dword ptr ss:[ebx],eax
mov eax,5
shl eax,2
add eax,12
mov ebx,ebp
sub ebx,eax
mov eax,65
mov dword ptr ss:[ebx],eax
mov eax,6
shl eax,2
add eax,12
mov ebx,ebp
sub ebx,eax
mov eax,70
mov dword ptr ss:[ebx],eax
mov eax,0
mov dword ptr ss:[ebp-40],eax
mov eax,0
mov dword ptr ss:[ebp-40],eax
LABEL0:
mov eax,dword ptr ss:[ebp-40]
cmp eax,7
jge LABEL2
mov eax,dword ptr ss:[ebp-40]
add eax,1
mov dword ptr ss:[ebp-52],eax
LABEL1:
mov eax,dword ptr ss:[ebp-40]
mov dword ptr ss:[ebp-44],eax
LABEL3:
mov eax,dword ptr ss:[ebp-44]
cmp eax,7
jge LABEL5
mov eax,dword ptr ss:[ebp-44]
add eax,1
mov dword ptr ss:[ebp-56],eax
LABEL4:
mov eax,dword ptr ss:[ebp-40]
shl eax,2
add eax,12
mov ebx,ebp
sub ebx,eax
mov dword ptr ss:[ebp-60],ebx
mov eax,dword ptr ss:[ebp-44]
shl eax,2
add eax,12
mov ebx,ebp
sub ebx,eax
mov eax,dword ptr ss:[ebp-60]
mov ecx,dword ptr ss:[eax]
cmp ecx,dword ptr ss:[ebx]
jge LABEL7
LABEL6:
mov eax,dword ptr ss:[ebp-40]
shl eax,2
add eax,12
mov ebx,ebp
sub ebx,eax
mov eax,dword ptr ss:[ebx]
mov dword ptr ss:[ebp-48],eax
mov eax,dword ptr ss:[ebp-40]
shl eax,2
add eax,12
mov ebx,ebp
sub ebx,eax
mov dword ptr ss:[ebp-64],ebx
mov eax,dword ptr ss:[ebp-44]
shl eax,2
add eax,12
mov ebx,ebp
sub ebx,eax
mov eax,dword ptr ss:[ebp-64]
mov ecx,dword ptr ss:[ebx]
mov dword ptr ss:[eax],ecx
mov eax,dword ptr ss:[ebp-44]
shl eax,2
add eax,12
mov ebx,ebp
sub ebx,eax
mov eax,dword ptr ss:[ebp-48]
mov dword ptr ss:[ebx],eax
LABEL7:
mov eax,dword ptr ss:[ebp-56]
mov dword ptr ss:[ebp-44],eax
jmp LABEL3
LABEL5:
mov eax,dword ptr ss:[ebp-52]
mov dword ptr ss:[ebp-40],eax
jmp LABEL0
LABEL2:
mov eax,0
mov dword ptr ss:[ebp-40],eax
LABEL9:
mov eax,dword ptr ss:[ebp-40]
cmp eax,7
jge LABEL11
mov eax,dword ptr ss:[ebp-40]
add eax,1
mov dword ptr ss:[ebp-68],eax
LABEL10:
mov eax,dword ptr ss:[ebp-40]
shl eax,2
add eax,12
mov ebx,ebp
sub ebx,eax
mov eax,dword ptr ss:[ebx]
mov dword ptr ss:[ebp-48],eax
push dword ptr ss:[ebp-48]
lea eax,szFormat
push eax
call crt_printf
mov eax,dword ptr ss:[ebp-68]
mov dword ptr ss:[ebp-40],eax
jmp LABEL9
LABEL11:
mov eax,dword ptr ss:[ebp-40]
add esp,72
mov esp,ebp
pop ebp
ret
sort endp
start:
push ebp
mov ebp,esp
sub esp,12
call sort
add esp,12
mov esp,ebp
pop ebp
ret
end start
