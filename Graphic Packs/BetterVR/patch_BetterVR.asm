[BotW_BetterVR_V208]
moduleMatches = 0x6267BFD0

.origin = codecave

; Data from the native application
vrStatus:
.byte 1
.align 4
headsetQuaternion:
.float 0.0
.float 0.0
.float 0.0
.float 0.0
headsetPosition:
.float 0.0
.float 0.0
.float 0.0

; Inputs for the calculation code cave
oldCamPos:
.float 0.0
.float 0.0
.float 0.0
oldTargetPos:
.float 0.0
.float 0.0
.float 0.0
; Output from the calculation code cave
newCamPos:
.float 0.0
.float 0.0
.float 0.0
newTargetPos:
.float 0.0
.float 0.0
.float 0.0
newCamRot:
.float 0.0
.float 0.0
.float 0.0

; Additional settings you can change
FOVSetting:
.float $FOV
DistanceSetting:
.float 1.0
RotationSetting:
.float 1.0


storeLRWrapper:
.int 0

CAM_OFFSET_POS = 0x5C0
CAM_OFFSET_TARGET = 0x5CC

changeCameraMatrix:
lwz r0, 0x1c(r1) ; original instruction

lis r7, vrStatus@ha
lbz r7, vrStatus@l(r7)
cmpwi r7, 2
bnelr

lfs f0, CAM_OFFSET_POS+0x0(r31)
lis r7, oldCamPos@ha
stfs f0, oldCamPos@l+0x0(r7)

lfs f0, CAM_OFFSET_POS+0x4(r31)
lis r7, oldCamPos@ha
stfs f0, oldCamPos@l+0x4(r7)

lfs f0, CAM_OFFSET_POS+0x8(r31)
lis r7, oldCamPos@ha
stfs f0, oldCamPos@l+0x8(r7)

lfs f0, CAM_OFFSET_TARGET+0x0(r31)
lis r7, oldTargetPos@ha
stfs f0, oldTargetPos@l+0x0(r7)

lfs f0, CAM_OFFSET_TARGET+0x4(r31)
lis r7, oldTargetPos@ha
stfs f0, oldTargetPos@l+0x4(r7)

lfs f0, CAM_OFFSET_TARGET+0x8(r31)
lis r7, oldTargetPos@ha
stfs f0, oldTargetPos@l+0x8(r7)

mflr r7
lis r10, storeLRWrapper@ha
stw r7, storeLRWrapper@l(r10)
bla calcNewRotationWrapper
lis r10, storeLRWrapper@ha
lwz r7, storeLRWrapper@l(r10)
mtlr r7

lis r7, newCamPos@ha
lfs f0, newCamPos@l+0x0(r7)
stfs f0, CAM_OFFSET_POS(r31)

lis r7, newCamPos@ha
lfs f0, newCamPos@l+0x4(r7)
stfs f0, CAM_OFFSET_POS+0x4(r31)

lis r7, newCamPos@ha
lfs f0, newCamPos@l+0x8(r7)
stfs f0, CAM_OFFSET_POS+0x8(r31)

lis r7, newTargetPos@ha
lfs f0, newTargetPos@l+0x0(r7)
stfs f0, CAM_OFFSET_TARGET+0x0(r31)

lis r7, newTargetPos@ha
lfs f0, newTargetPos@l+0x4(r7)
stfs f0, CAM_OFFSET_TARGET+0x4(r31)

lis r7, newTargetPos@ha
lfs f0, newTargetPos@l+0x8(r7)
stfs f0, CAM_OFFSET_TARGET+0x8(r31)

lis r7, FOVSetting@ha
lfs f0, FOVSetting@l(r7)
stfs f0, 0x5E4(r31)

blr

0x02C05500 = bla changeCameraMatrix
0x02C05598 = bla changeCameraMatrix

changeCameraRotation:
stfs f10, 0x18(r31)

lis r8, vrStatus@ha
lbz r8, vrStatus@l(r8)
cmpwi r8, 2
bnelr

lis r8, newCamRot@ha
lfs f10, newCamRot@l+0x0(r8)
stfs f10, 0x18(r31)

lis r8, newCamRot@ha
lfs f10, newCamRot@l+0x4(r8)
stfs f10, 0x1C(r31)

lis r8, newCamRot@ha
lfs f10, newCamRot@l+0x8(r8)
stfs f10, 0x20(r31)

blr

0x02E57FF0 = bla changeCameraRotation