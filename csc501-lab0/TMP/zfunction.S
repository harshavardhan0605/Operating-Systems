	.text
	.global zfunction

zfunction:
	pushl %ebp
	movl %esp,%ebp
	movl 8(%ebp),%eax
	AND $0xff803fff,%eax
	shl $4,%eax
	leave
	ret
