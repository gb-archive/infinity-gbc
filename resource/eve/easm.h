#define align(x,y)      db 0x00, x, y
#define say(msg)     db 0x01 __NL__ dw msg
#define group        db 0x02
#define walk(slot,dir,num) db 0x03, slot, dir,  num
#define walk2(slot,dir,num) db 0x64, slot, dir,  num

;  { "move",   3, 0, 4 },  // dude, direction, distance
;  { "push",   3, 0, 5 },  // dude, direction, distance

#define warp(slot,x,y,dir) db 0x06, slot, x, y, dir

#define face(slot,dir)     db 0x07, slot, dir

;  { "frame",  2, 0, 8 },  // dude, frame

#define lockon(guy)     db 0x34, guy
#define pan(x,y)     db 0x35, x, y
#define lockoff         db 0x36
#define camera(x,y)     db 0x37, x, y

#define resume       db 0x11
#define exit         db 0x12

#define sync         db 0x13
#define sleep(loops)    db 0x14, loops

#define ldude(slot,t,x,y,d,c) db 0x09, slot, t, x,y,d,c
#define lmap(map,x,y)      db 0x0A, map,  x, y
#define del(slot)    db 0x0B, slot
#define lguy(t,x,y,msg)    db 0x0C, t, x,y __NL__ dw msg

#define cstep(eve,x,y)     db 0x1F __NL__ dw eve __NL__ db x,y
#define ctalk(eve,slot)    db 0x20 __NL__ dw eve __NL__ db slot
#define csearch(eve,x,y,srdir)   db 0x21 __NL__ dw eve __NL__ db x,y,srdir
#define chstep(eve,x,y,len)   db 0x22 __NL__ dw eve __NL__ db x,y,len
#define cvstep(eve,x,y,len)   db 0x23 __NL__ dw eve __NL__ db x,y,len
#define cqstep(eve,x,y)       db 0x5f __NL__ dw eve __NL__ db x,y
#define ctres(flag,i,x,y,g,s) db 0x53, flag, x, y,g,s,i

#define song(n)         db 0x24, n
#define bsong(n)        db 0x81, n
#define sfx(n)          db 0x48, n

#define control(n)      db 0x25, n
#define goto(label)     db 0x27 __NL__ dw label
#define call(label)     db 0x49 __NL__ dw label

#define fadeout(type)         db 0x28, type
#define tile(x,y,num)      db 0x29, x, y, num

; logic
; regs only
#define if(op1,operand,op2,eve)  db 0x15, op1,  operand, op2 __NL__ dw eve
#define cif(op1,operand,op2,eve) db 0x57, op1,  operand, op2 __NL__ dw eve

; reg, imm
#define set(reg,val)    db 0x16, reg,  val

; reg, reg/imm
#define add(op1,op2)    db 0x17, op1,  op2
#define sub(op1,op2)    db 0x18, op1,  op2
#define mul(op1,op2)    db 0x19, op1,  op2
#define div(op1,op2)    db 0x1a, op1,  op2

; reg
#define inc(reg)     db 0x1b, reg
#define dec(reg)     db 0x1c, reg

#define get(reg,var)    db 0x1d, reg __NL__ dw var
#define put(reg,var)    db 0x1e, reg __NL__ dw var


#define give(item)      db 0x30, item
#define take(item)      db 0x31, item
#define have(reg,item)     db 0x32, reg,item

#define partyset(slot,guy) db 0x40, slot,guy
#define partynor     db 0x41
#define partyarr     db 0x42

#define panspeed(speed)    db 0x43, speed
#define fadein(type)       db 0x44, type

#define frame(slot,num)    db 0x45, slot, num
#define unframe(slot)      db 0x80, slot
#define reform       db 0x46
#define detach       db 0x47

#define cgold(reg,money)  db 0x4A,reg __NL__ dw money
#define tgold(money)  db 0x4B __NL__ dw money

#define shop_res          db 0x4C
#define shop_add(i)       db 0x4D,i
#define shop_add2(i,cost) db 0x79,i __NL__ dw cost

#define shop              db 0x4E

#define yesno(reg)        db 0x4F,reg
#define savemenu          db 0x50

#define zone(x)           db 0x51,x
#define getxy(regx,regy)  db 0x52,regx,regy

#define healall           db 0x54
#define options           db 0x55
#define colshift(color)   db 0x56,color

#define prophecy          db 0x58
#define tbase(bas,len,mul)db 0x59 __NL__ dw bas __NL__ db len, mul

#define battle(f,r,l,g,h) db 0x5a,f,r,l,g,h
#define song_off          db 0x5b

#define setskl(guy,slot,x)db 0x5c,guy,slot,x

#define song_pause        db 0x5d
#define song_resume       db 0x5e
#define givegem(gem)      db 0x60,gem
#define revive            db 0x61
#define setform           db 0x62

#define equip(guy,item)   db 0x63,guy,item

#define boat_on           db 0x65
#define boat_off          db 0x66

#define mapstate(state)   db 0x67,state
#define hitstate(state)   db 0x68,state

#define quake             db 0x69
#define setz(guy,z)       db 0x70,guy,z

#define setwolf(val)      db 0x71,val
#define rand(reg,val)     db 0x72,reg,val

#define fadein2(type)     db 0x73,type
#define panrel(x,y)       db 0x74,x,y

#define palset(x)         db 0x75,x
#define palnorm           db 0x76
#define levelup(guy)      db 0x77,guy

#define setboat(x,y)      db 0x78,x,y
#define setwolfp(x)       db 0x7a,x

#define give2(item)       db 0x7b, item
#define setskl2(guy,slot,x) db 0x7c,guy,slot,x

#define anim(slot)        db 0x7d,slot
#define warp2(slot,x,y,dir)   db 0x7e, slot, x, y, dir
#define ldudehi(slot,type,x,y,frame) db 0x7f, slot, type, x, y, frame
#define darken            db 0x6a
#define colshift2(col)    db 0x6b,col
#define dequip(guy,slot)  db 0x6c,guy,slot
#define sayauto(x)     db 0x6d,x
#define lastmoves(x1,x2,x3) db 0x6e,x1,x2,x3
#define drawdudes         db 0x6f
#define reset             db 0x2a
#define endinit        db 0x2b
#define endnext        db 0x2c
#define fadefinal         db 0x2d
#define lock        db 0x2e
#define poem(x)        db 0x2f,x
#define setbosspal(x)     db 0x38,x

#define end      db 0xFF

#define A   0xF8
#define B   0xF9
#define C   0xFA
#define D   0xFB
#define E   0xFC
#define F   0xFD
#define G   0xFE
#define H   0xFF

#define CG  0x00
#define CGE 0x01
#define CE  0x02
#define CLE 0x03
#define CL  0x04
#define CNE 0x05

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

   org  0

;   end

