[BetterVR_UpdateProjectionMatrix_V208]
moduleMatches = 0x6267BFD0

.origin = codecave


0x030C16D0 = sead_PerspectiveProjection_setFovy:

default_AspectRatio:
.float 1.77777779
default_FovY:
.float 1.04719758
default_OffsetX:
.float 0.0
default_OffsetY:
.float 0.0

ASPECT_RATIO_STACK_OFFSET = 0x14
FOVY_STACK_OFFSET = 0x18
OFFSET_X_STACK_OFFSET = 0x1C
OFFSET_Y_STACK_OFFSET = 0x20

updateLookAtProjectionMatrix:
mflr r0
stwu r1, -0x28(r1)
stw r0, 0x2C(r1)

mr r0, r28
lis r28, default_AspectRatio@ha
lfs f0, default_AspectRatio@l(r28)
stfs f0, ASPECT_RATIO_STACK_OFFSET(r1)
lis r28, default_FovY@ha
lfs f0, default_FovY@l(r28)
stfs f0, FOVY_STACK_OFFSET(r1)
lis r28, default_OffsetX@ha
lfs f0, default_OffsetX@l(r28)
stfs f0, OFFSET_X_STACK_OFFSET(r1)
lis r28, default_OffsetY@ha
lfs f0, default_OffsetY@l(r28)
stfs f0, OFFSET_Y_STACK_OFFSET(r1)
mr r28, r0

addi r28, r1, 0x14
bl import.coreinit.hook_UpdateProjectionMatrix

; store dirty flag
li r28, 1
stb r28, 8(r30)

; store offsetX
lfs f0, OFFSET_X_STACK_OFFSET(r1)
stfs f0, 0xB8(r30)

; store offsetY
lfs f12, OFFSET_Y_STACK_OFFSET(r1)
stfs f12, 0xBC(r30)

; set fovY
lfs f1, FOVY_STACK_OFFSET(r1)
addi r3, r30, 8
bl sead_PerspectiveProjection_setFovy

; store dirty flag
li r28, 1
stb r28, 8(r30)

; store near
lfs f13, 0x60(r31)
stfs f13, 0x9C(r30)

; store far
lfs f0, 0x64(r31)
stfs f0, 0xA0(r30)

; store aspectRatio
lfs f12, ASPECT_RATIO_STACK_OFFSET(r1)
stfs f12, 0xB4(r30)


lwz r0, 0x2C(r1)
mtlr r0
addi r1, r1, 0x28
blr

0x0386CFD0 = b skipExistingCode
0x0386D024 = skipExistingCode:
0x0386D024 = bla updateLookAtProjectionMatrix
