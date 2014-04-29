
src/0.bin:     file format elf32-i386


Disassembly of section .text:

00000000 <printf>:
   0:	55                   	push   %ebp
   1:	89 e5                	mov    %esp,%ebp
   3:	89 ec                	mov    %ebp,%esp
   5:	5d                   	pop    %ebp
   6:	58                   	pop    %eax
   7:	a3 68 01 00 00       	mov    %eax,0x168
   c:	b8 32 71 10 00       	mov    $0x107132,%eax
  11:	50                   	push   %eax
  12:	9a ad 7b 10 00 08 00 	lcall  $0x8,$0x107bad
  19:	a1 68 01 00 00       	mov    0x168,%eax
  1e:	50                   	push   %eax
  1f:	c3                   	ret    
  20:	b8 00 00 00 00       	mov    $0x0,%eax
  25:	5d                   	pop    %ebp
  26:	c3                   	ret    

00000027 <main>:
  27:	55                   	push   %ebp
  28:	89 e5                	mov    %esp,%ebp
  2a:	83 e4 f0             	and    $0xfffffff0,%esp
  2d:	83 ec 20             	sub    $0x20,%esp
  30:	c7 44 24 08 01 01 00 	movl   $0x101,0x8(%esp)
  37:	00 
  38:	c7 44 24 04 3a 01 00 	movl   $0x13a,0x4(%esp)
  3f:	00 
  40:	c7 04 24 7b 00 00 00 	movl   $0x7b,(%esp)
  47:	e8 29 00 00 00       	call   75 <pow>
  4c:	89 44 24 1c          	mov    %eax,0x1c(%esp)
  50:	8b 44 24 1c          	mov    0x1c(%esp),%eax
  54:	89 44 24 04          	mov    %eax,0x4(%esp)
  58:	c7 04 24 eb 00 00 00 	movl   $0xeb,(%esp)
  5f:	e8 9c ff ff ff       	call   0 <printf>
  64:	8b 44 24 1c          	mov    0x1c(%esp),%eax
  68:	89 c0                	mov    %eax,%eax
  6a:	89 ec                	mov    %ebp,%esp
  6c:	5d                   	pop    %ebp
  6d:	cb                   	lret   
  6e:	b8 00 00 00 00       	mov    $0x0,%eax
  73:	c9                   	leave  
  74:	c3                   	ret    

00000075 <pow>:
  75:	55                   	push   %ebp
  76:	89 e5                	mov    %esp,%ebp
  78:	83 ec 28             	sub    $0x28,%esp
  7b:	8b 45 08             	mov    0x8(%ebp),%eax
  7e:	99                   	cltd   
  7f:	f7 7d 10             	idivl  0x10(%ebp)
  82:	89 55 08             	mov    %edx,0x8(%ebp)
  85:	83 7d 0c 00          	cmpl   $0x0,0xc(%ebp)
  89:	75 0b                	jne    96 <pow+0x21>
  8b:	8b 45 08             	mov    0x8(%ebp),%eax
  8e:	99                   	cltd   
  8f:	f7 7d 10             	idivl  0x10(%ebp)
  92:	89 d0                	mov    %edx,%eax
  94:	eb 53                	jmp    e9 <pow+0x74>
  96:	8b 45 0c             	mov    0xc(%ebp),%eax
  99:	89 c2                	mov    %eax,%edx
  9b:	c1 ea 1f             	shr    $0x1f,%edx
  9e:	01 d0                	add    %edx,%eax
  a0:	d1 f8                	sar    %eax
  a2:	89 c1                	mov    %eax,%ecx
  a4:	8b 45 08             	mov    0x8(%ebp),%eax
  a7:	0f af 45 08          	imul   0x8(%ebp),%eax
  ab:	99                   	cltd   
  ac:	f7 7d 10             	idivl  0x10(%ebp)
  af:	89 d0                	mov    %edx,%eax
  b1:	8b 55 10             	mov    0x10(%ebp),%edx
  b4:	89 54 24 08          	mov    %edx,0x8(%esp)
  b8:	89 4c 24 04          	mov    %ecx,0x4(%esp)
  bc:	89 04 24             	mov    %eax,(%esp)
  bf:	e8 b1 ff ff ff       	call   75 <pow>
  c4:	89 45 f4             	mov    %eax,-0xc(%ebp)
  c7:	8b 45 0c             	mov    0xc(%ebp),%eax
  ca:	83 e0 01             	and    $0x1,%eax
  cd:	85 c0                	test   %eax,%eax
  cf:	74 0f                	je     e0 <pow+0x6b>
  d1:	8b 45 08             	mov    0x8(%ebp),%eax
  d4:	0f af 45 f4          	imul   -0xc(%ebp),%eax
  d8:	99                   	cltd   
  d9:	f7 7d 10             	idivl  0x10(%ebp)
  dc:	89 d0                	mov    %edx,%eax
  de:	eb 09                	jmp    e9 <pow+0x74>
  e0:	8b 45 f4             	mov    -0xc(%ebp),%eax
  e3:	99                   	cltd   
  e4:	f7 7d 10             	idivl  0x10(%ebp)
  e7:	89 d0                	mov    %edx,%eax
  e9:	c9                   	leave  
  ea:	c3                   	ret    
