[BetterVR_Find3DFrameBuffer_V208]
moduleMatches = 0x6267BFD0

.origin = codecave

str_printClear3DColorBuffer_left:
.string "[PPC] Clearing 3D color buffer with left eye"
str_printClear3DColorBuffer_right:
.string "[PPC] Clearing 3D color buffer with right eye"

magic3DColorValue:
.float (0.0 / 32.0)
.float 0.123456789
.float 0.987654321
magic3DColorValue_leftSide:
.float 0.0
magic3DColorValue_rightSide:
.float 1.0

clear3DColorBuffer:
mflr r6
stwu r1, -0x20(r1)
stw r6, 0x24(r1)

stfs f1, 0x20(r1)
stfs f2, 0x1C(r1)
stfs f3, 0x18(r1)
stfs f4, 0x14(r1)
stw r3, 0x10(r1)

lis r3, magic3DColorValue@ha
lfs f1, magic3DColorValue@l+0x0(r3)
lfs f2, magic3DColorValue@l+0x4(r3)
lfs f3, magic3DColorValue@l+0x8(r3)

lis r3, currentEyeSide@ha
lwz r3, currentEyeSide@l(r3)
cmpwi r3, 0
lis r3, magic3DColorValue_leftSide@ha
lfs f4, magic3DColorValue_leftSide@l(r3)
beq continueWithClear
lis r3, magic3DColorValue_rightSide@ha
lfs f4, magic3DColorValue_rightSide@l(r3)

continueWithClear:
mr r3, r26
addi r3, r3, 0x1C ; r3 is now the agl::RenderBuffer::mColorBuffer array
lwz r3, 0(r3) ; r3 is now the agl::RenderBuffer::mColorBuffer[0] object
cmpwi r3, 0
beq skipClearing3DColorBuffer
addi r3, r3, 0xBC ; r3 is now the agl::RenderBuffer::mColorBuffer[0]::mGX2FrameBuffer object

bl import.gx2.GX2ClearColor
bla import.gx2.GX2DrawDone
bla import.gx2.GX2Flush
bla import.gx2.GX2DrawDone

stwu r1, -0x20(r1)
stw r3, 0x1C(r1)
stw r4, 0x18(r1)
stw r5, 0x14(r1)
stw r6, 0x10(r1)

lis r3, currentEyeSide@ha
lwz r3, currentEyeSide@l(r3)
cmpwi r3, 0
lis r3, str_printClear3DColorBuffer_left@ha
addi r3, r3, str_printClear3DColorBuffer_left@l
beq actualPrint_1

lis r3, str_printClear3DColorBuffer_right@ha
addi r3, r3, str_printClear3DColorBuffer_right@l
actualPrint_1:
li r4, 10
crxor 4*cr1+eq, 4*cr1+eq, 4*cr1+eq
bl import.coreinit.hook_OSReportToConsole

lwz r3, 0x1C(r1)
lwz r4, 0x18(r1)
lwz r5, 0x14(r1)
lwz r6, 0x10(r1)
addi r1, r1, 0x20

;bla import.gx2.GX2DrawDone
;bla import.gx2.GX2Flush
;bla import.gx2.GX2DrawDone


skipClearing3DColorBuffer:
lfs f1, 0x20(r1)
lfs f2, 0x1C(r1)
lfs f3, 0x18(r1)
lfs f4, 0x14(r1)
lwz r3, 0x10(r1)

lwz r6, 0x24(r1)
mtlr r6
addi r1, r1, 0x20
blr


magic3DDepthValue:
.float 0.0123456789

str_printClear3DDepthBuffer_left:
.string "[PPC] Clearing 3D depth buffer with left eye"
str_printClear3DDepthBuffer_right:
.string "[PPC] Clearing 3D depth buffer with right eye"

clear3DDepthBuffer:
mflr r3
stwu r1, -0x20(r1)
stw r3, 0x24(r1)

stfs f1, 0x20(r1)
stw r3, 0x10(r1)
stw r4, 0x14(r1)
stw r5, 0x18(r1)

lis r3, magic3DDepthValue@ha
lfs f1, magic3DDepthValue@l+0x0(r3)
li r4, 1
lis r5, currentEyeSide@ha
lwz r5, currentEyeSide@l(r5)
add r4, r4, r5
li r5, 3

mr r3, r26
addi r3, r3, 0x3C ; r3 is now the agl::RenderBuffer::mDepthTarget array
lwz r3, 0(r3) ; r3 is now the agl::RenderBuffer::mDepthTarget object
cmpwi r3, 0
beq skipClearing3DDepthBuffer
addi r3, r3, 0xBC ; r3 is now the agl::RenderBuffer::mDepthTarget::mGX2FrameBuffer object

bl import.gx2.GX2ClearDepthStencilEx
;bla import.gx2.GX2DrawDone
;bla import.gx2.GX2Flush
;bla import.gx2.GX2DrawDone

stwu r1, -0x20(r1)
stw r3, 0x1C(r1)
stw r4, 0x18(r1)
stw r5, 0x14(r1)
stw r6, 0x10(r1)

lis r3, currentEyeSide@ha
lwz r3, currentEyeSide@l(r3)
cmpwi r3, 0
lis r3, str_printClear3DDepthBuffer_left@ha
addi r3, r3, str_printClear3DDepthBuffer_left@l
beq actualPrint_2

lis r3, str_printClear3DDepthBuffer_right@ha
addi r3, r3, str_printClear3DDepthBuffer_right@l
actualPrint_2:
li r4, 10
crxor 4*cr1+eq, 4*cr1+eq, 4*cr1+eq
bl import.coreinit.hook_OSReportToConsole

lwz r3, 0x1C(r1)
lwz r4, 0x18(r1)
lwz r5, 0x14(r1)
lwz r6, 0x10(r1)
addi r1, r1, 0x20

bla import.gx2.GX2DrawDone
bla import.gx2.GX2Flush
bla import.gx2.GX2DrawDone

skipClearing3DDepthBuffer:
lfs f1, 0x20(r1)
lwz r3, 0x10(r1)
lwz r4, 0x14(r1)
lwz r5, 0x18(r1)

lwz r3, 0x24(r1)
mtlr r3
addi r1, r1, 0x20
blr


; r10 holds the agl::RenderBuffer object
hookPostHDRComposedImage:
mflr r3
stwu r1, -0x4(r1)
stw r3, 0x8(r1)

bla clear3DColorBuffer
bla clear3DDepthBuffer

lwz r3, 0x8(r1)
mtlr r3
addi r1, r1, 0x4

lmw r18, 0xB0(r1) ; original instruction
blr


0x039B3044 = bla hookPostHDRComposedImage
;0x0397AB30 = cmpw r3, r3