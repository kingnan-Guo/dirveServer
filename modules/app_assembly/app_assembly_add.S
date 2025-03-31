.text
.global app_assembly_add

app_assembly_add:
    add x0, x0, x1  // x0 = x0 + x1 (AArch64 使用 64 位寄存器 x0, x1)
    ret             // 返回