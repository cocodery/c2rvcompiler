/**
 * @name Fast Small Thread Library
 * @brief a risc thread lib for automatic parallelization
 * @author 3tle3wa
 *
 * @attention please follow the license
 */

const char *libfstl = R"(# experimental
	.data
	.align	4
	.type	.LC.tnum, @object
	.size	.LC.tnum, 4
.LC.tnum:
	.zero	4
	.text
	.align	1
	.type	__crvc_fork, @function
	.global	__crvc_fork
__crvc_fork:
	lla	t0, .LC.tnum
	li	t1, 1
	amoadd.w.aqrl	zero, t1, 0(t0)
	li	a7, 220
	mv	a5, zero
	mv	a4, zero
	mv	a3, zero
	mv	a2, zero
	mv	a1, sp
	li	a0, 256
	ecall
	ret
	.size	__crvc_fork, .-__crvc_fork
	.text
	.align	1
	.type	__crvc_exit, @function
	.global	__crvc_exit
__crvc_exit:
	lla	t0, .LC.tnum
	li	t1, -1
	amoadd.w.aqrl	zero, t1, 0(t0)
	li	a7, 93
	li	a0, 0
	ecall
	.size	__crvc_exit, .-__crvc_exit
	.text
	.align	1
	.type	__crvc_pass, @function
	.global	__crvc_pass
__crvc_pass:
	lla	t0, .LC.tnum
.trypass:
	li	a7, 158
	ecall
	lw	t1, 0(t0)
	bnez	t1, .trypass
	ret
	.size	__crvc_pass, .-__crvc_pass
)";

const char *libfstl2 = R"(# experimental
	.data
	.align	4
	.type	.LC.tcreated, @object
	.size	.LC.tcreated, 4
.LC.tcreated:
	.zero	4
	.data
	.align	4
	.type	.LC.texited, @object
	.size	.LC.texited, 4
.LC.texited:
	.zero	4
	.text
	.align	1
	.type	__crvc_fork, @function
	.global	__crvc_fork
__crvc_fork:
	lw	a0, .LC.tcreated
	addi	a0, a0, 1
	sw	a0, .LC.tcreated, a1
	li	a7, 220
	mv	a5, zero
	mv	a4, zero
	mv	a3, zero
	mv	a2, zero
	mv	a1, sp
	li	a0, 256
	ecall
	ret
	.size	__crvc_fork, .-__crvc_fork
	.text
	.align	1
	.type	__crvc_exit, @function
	.global	__crvc_exit
__crvc_exit:
	lla	a0, .LC.texited
	li	a1, 1
	amoadd.w.aqrl	zero, a1, 0(a0)
	li	a7, 93
	li	a0, 0
	ecall
	.size	__crvc_exit, .-__crvc_exit
	.text
	.align	1
	.type	__crvc_pass, @function
	.global	__crvc_pass
__crvc_pass:
	ld	a1, .LC.tcreated
	lla	a2, .LC.texited
.trypass:
	li	a7, 158
	ecall
	amoor.w.aqrl	a3, zero, 0(a2)
	bne	a3, a1, .trypass
	ret
	.size	__crvc_pass, .-__crvc_pass
)";