#define align(x,y)      .db $00, x, y
#define say(msg)     .db $01 \ .dw msg
#define group        .db $02
#define walk(slot,dir,num) .db $03, slot, dir,  num
#define walk2(slot,dir,num) .db $64, slot, dir,  num

;  { "move",   3, 0, 4 },  // dude, direction, distance
;  { "push",   3, 0, 5 },  // dude, direction, distance

#define warp(slot,x,y,dir) .db $06, slot, x, y, dir

#define face(slot,dir)     .db $07, slot, dir

;  { "frame",  2, 0, 8 },  // dude, frame

#define lockon(guy)     .db $34, guy
#define pan(x,y)     .db $35, x, y
#define lockoff         .db $36
#define camera(x,y)     .db $37, x, y

#define resume       .db $11
#define exit         .db $12

#define sync         .db $13
#define sleep(loops)    .db $14, loops

#define ldude(slot,t,x,y,d,c) .db $09, slot, t, x,y,d,c
#define lmap(map,x,y)      .db $0A, map,  x, y
#define del(slot)    .db $0B, slot
#define lguy(t,x,y,msg)    .db $0C, t, x,y \ .dw msg

#define cstep(eve,x,y)     .db $1F \ .dw eve \ .db x,y
#define ctalk(eve,slot)    .db $20 \ .dw eve \ .db slot
#define csearch(eve,x,y,srdir)   .db $21 \ .dw eve \ .db x,y,srdir
#define chstep(eve,x,y,len)   .db $22 \ .dw eve \ .db x,y,len
#define cvstep(eve,x,y,len)   .db $23 \ .dw eve \ .db x,y,len
#define cqstep(eve,x,y)       .db $5f \ .dw eve \ .db x,y
#define ctres(flag,i,x,y,g,s) .db $53, flag, x, y,g,s,i

#define song(n)         .db $24, n
#define bsong(n)        .db $81, n
#define sfx(n)          .db $48, n

#define control(n)      .db $25, n
#define goto(label)     .db $27 \ .dw label
#define call(label)     .db $49 \ .dw label

#define fadeout(type)         .db $28, type
#define tile(x,y,num)      .db $29, x, y, num

; logic
; regs only
#define if(op1,operand,op2,eve)  .db $15, op1,  operand, op2 \ .dw eve
#define cif(op1,operand,op2,eve) .db $57, op1,  operand, op2 \ .dw eve

; reg, imm
#define set(reg,val)    .db $16, reg,  val

; reg, reg/imm
#define add(op1,op2)    .db $17, op1,  op2
#define sub(op1,op2)    .db $18, op1,  op2
#define mul(op1,op2)    .db $19, op1,  op2
#define div(op1,op2)    .db $1a, op1,  op2

; reg
#define inc(reg)     .db $1b, reg
#define dec(reg)     .db $1c, reg

#define get(reg,var)    .db $1d, reg \ .dw var
#define put(reg,var)    .db $1e, reg \ .dw var


#define give(item)      .db $30, item
#define take(item)      .db $31, item
#define have(reg,item)     .db $32, reg,item

#define partyset(slot,guy) .db $40, slot,guy
#define partynor     .db $41
#define partyarr     .db $42

#define panspeed(speed)    .db $43, speed
#define fadein(type)       .db $44, type

#define frame(slot,num)    .db $45, slot, num
#define unframe(slot)      .db $80, slot
#define reform       .DB $46
#define detach       .DB $47

#define cgold(reg,money)  .db $4A,reg \ .dw money
#define tgold(money)  .db $4B \ .dw money

#define shop_res          .db $4C
#define shop_add(i)       .db $4D,i
#define shop_add2(i,cost) .db $79,i \ .dw cost

#define shop              .db $4E

#define yesno(reg)        .db $4F,reg
#define savemenu          .db $50

#define zone(x)           .db $51,x
#define getxy(regx,regy)  .db $52,regx,regy

#define healall           .db $54
#define options           .db $55
#define colshift(color)   .db $56,color

#define prophecy          .db $58
#define tbase(bas,len,mul).db $59 \ .dw bas \ .db len, mul

#define battle(f,r,l,g,h) .db $5a,f,r,l,g,h
#define song_off          .db $5b

#define setskl(guy,slot,x).db $5c,guy,slot,x

#define song_pause        .db $5d
#define song_resume       .db $5e
#define givegem(gem)      .db $60,gem
#define revive            .db $61
#define setform           .db $62

#define equip(guy,item)   .db $63,guy,item

#define boat_on           .db $65
#define boat_off          .db $66

#define mapstate(state)   .db $67,state
#define hitstate(state)   .db $68,state

#define quake             .db $69
#define setz(guy,z)       .db $70,guy,z

#define setwolf(val)      .db $71,val
#define rand(reg,val)     .db $72,reg,val

#define fadein2(type)     .db $73,type
#define panrel(x,y)       .db $74,x,y

#define palset(x)         .db $75,x
#define palnorm           .db $76
#define levelup(guy)      .db $77,guy

#define setboat(x,y)      .db $78,x,y
#define setwolfp(x)       .db $7a,x

#define give2(item)       .db $7b, item
#define setskl2(guy,slot,x) .db $7c,guy,slot,x

#define anim(slot)        .db $7d,slot
#define warp2(slot,x,y,dir)   .db $7e, slot, x, y, dir
#define ldudehi(slot,type,x,y,frame) .db $7f, slot, type, x, y, frame
#define darken            .db $6a
#define colshift2(col)    .db $6b,col
#define dequip(guy,slot)  .db $6c,guy,slot
#define sayauto(x)     .db $6d,x
#define lastmoves(x1,x2,x3) .db $6e,x1,x2,x3
#define drawdudes         .db $6f
#define reset             .db $2a
#define endinit        .db $2b
#define endnext        .db $2c
#define fadefinal         .db $2d
#define lock        .db $2e
#define poem(x)        .db $2f,x
#define setbosspal(x)     .db $38,x

#define end      .db $FF

#define A   $F8
#define B   $F9
#define C   $FA
#define D   $FB
#define E   $FC
#define F   $FD
#define G   $FE
#define H   $FF

#define CG  $00
#define CGE $01
#define CE  $02
#define CLE $03
#define CL  $04
#define CNE $05

#define UP     0
#define DOWN   1
#define LEFT   2
#define RIGHT  3

#define PLAYER 0
#define PARTY  1
#define WALK   2
#define STAND  3

#define S_U    1
#define S_D    2
#define S_L    4
#define S_R    8
#define S_ALL  15

   .org  0

;   .end

