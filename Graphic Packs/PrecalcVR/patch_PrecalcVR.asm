[BotW_BetterVR_V208]
moduleMatches = 0x6267BFD0

.origin = codecave

newEnabled:
.byte 1
.align 4
newCamPos:
.float 0.0
.float 0.0
.float 0.0
newTargetPos:
.float 0.0
.float 0.0
.float 0.0
newRotation:
.float 0.0
.float 0.0
.float 0.0
newFOV:
.float 0.0

oldEnabled:
.byte 1
.align 4
oldCamPos:
.float 0.0
.float 0.0
.float 0.0
oldTargetPos:
.float 0.0
.float 0.0
.float 0.0
oldRotation:
.float 0.0
.float 0.0
.float 0.0
oldFOV:
.float 0.0

;0x4513FC58 = CAM_ADDRESS:

CAM_OFFSET_POS = 0x5C0 ; vec3
CAM_OFFSET_TARGET = 0x5CC ; vec3
; 0x5D8 vec3 ukn
; 0x5E4 float fov
; 0x5E8 24 * u32
;CAM_OFFSET_ROT = 0x678; was: 0x5E8 ; vec3 0x678 ??

changeCameraMatrix:
lwz r0, 0x1c(r1) ; original instruction

lis r7, newEnabled@ha
lbz r7, newEnabled@l(r7)
cmpwi r7, 2
bnelr

lfs f0, CAM_OFFSET_POS+0x0(r31)
lis r7, oldCamPos@ha
stfs f0, oldCamPos@l+0x0(r7)
lis r7, newCamPos@ha
lfs f0, newCamPos@l+0x0(r7)
stfs f0, CAM_OFFSET_POS(r31)

lfs f0, CAM_OFFSET_POS+0x4(r31)
lis r7, oldCamPos@ha
stfs f0, oldCamPos@l+0x4(r7)
lis r7, newCamPos@ha
lfs f0, newCamPos@l+0x4(r7)
stfs f0, CAM_OFFSET_POS+0x4(r31)

lfs f0, CAM_OFFSET_POS+0x8(r31)
lis r7, oldCamPos@ha
stfs f0, oldCamPos@l+0x8(r7)
lis r7, newCamPos@ha
lfs f0, newCamPos@l+0x8(r7)
stfs f0, CAM_OFFSET_POS+0x8(r31)

lfs f0, CAM_OFFSET_TARGET+0x0(r31)
lis r7, oldTargetPos@ha
stfs f0, oldTargetPos@l+0x0(r7)
lis r7, newTargetPos@ha
lfs f0, newTargetPos@l+0x0(r7)
stfs f0, CAM_OFFSET_TARGET+0x0(r31)

lfs f0, CAM_OFFSET_TARGET+0x4(r31)
lis r7, oldTargetPos@ha
stfs f0, oldTargetPos@l+0x4(r7)
lis r7, newTargetPos@ha
lfs f0, newTargetPos@l+0x4(r7)
stfs f0, CAM_OFFSET_TARGET+0x4(r31)

lfs f0, CAM_OFFSET_TARGET+0x8(r31)
lis r7, oldTargetPos@ha
stfs f0, oldTargetPos@l+0x8(r7)
lis r7, newTargetPos@ha
lfs f0, newTargetPos@l+0x8(r7)
stfs f0, CAM_OFFSET_TARGET+0x8(r31)

lfs f0, 0x5E4(r31)
lis r7, oldFOV@ha
stfs f0, oldFOV@l(r7)
lis r7, newFOV@ha
lfs f0, newFOV@l(r7)
stfs f0, 0x5E4(r31)

blr


;0x02C0884C = nop
;0x02C08864 = nop
;0x02C08868 = nop

0x02C05500 = bla changeCameraMatrix
0x02C05598 = bla changeCameraMatrix


changeCameraRotation:
stfs f10, 0x18(r31)

lis r8, newEnabled@ha
lbz r8, newEnabled@l(r8)
cmpwi r8, 2
bnelr

lfs f10, 0x18(r31)
lis r8, oldRotation@ha
stfs f10, oldRotation@l+0x0(r8)
lis r8, newRotation@ha
lfs f10, newRotation@l+0x0(r8)
stfs f10, 0x18(r31)

lfs f10, 0x1C(r31)
lis r8, oldRotation@ha
stfs f10, oldRotation@l+0x4(r8)
lis r8, newRotation@ha
lfs f10, newRotation@l+0x4(r8)
stfs f10, 0x1C(r31)

lfs f10, 0x20(r31)
lis r8, oldRotation@ha
stfs f10, oldRotation@l+0x8(r8)
lis r8, newRotation@ha
lfs f10, newRotation@l+0x8(r8)
stfs f10, 0x20(r31)

blr

0x02E57FF0 = bla changeCameraRotation

;0x02E57FF0 = nop ; rot x
;0x02E57FEC = nop ; rot y
;0x02E57FE8 = nop ; rot z

;0x02C08648 = nop ; rot x
;0x02C08634 = nop ; rot y
;0x02C08650 = nop ; rot z