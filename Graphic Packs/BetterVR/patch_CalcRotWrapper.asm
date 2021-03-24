[BotW_CalcRotWrapper_V208]
moduleMatches = 0x6267BFD0

.origin = codecave

storeLR:
.int 0

storeR1:
.int 0
storeR2:
.int 0
storeR3:
.int 0
storeR4:
.int 0
storeR31:
.int 0

storeF0:
.float 0
storeF1:
.float 0
storeF2:
.float 0
storeF3:
.float 0
storeF4:
.float 0
storeF5:
.float 0
storeF6:
.float 0
storeF7:
.float 0
storeF8:
.float 0
storeF9:
.float 0
storeF10:
.float 0
storeF11:
.float 0
storeF12:
.float 0
storeF13:
.float 0

storeF26:
.float 0
storeF27:
.float 0
storeF28:
.float 0
storeF29:
.float 0
storeF30:
.float 0
storeF31:
.float 0

calcNewRotationWrapper:
mflr r7
lis r10, storeLR@ha
stw r7, storeLR@l(r10)

lis r10, storeR1@ha
stw r1, storeR1@l(r10)
lis r10, storeR2@ha
stw r2, storeR2@l(r10)
lis r10, storeR3@ha
stw r3, storeR3@l(r10)
lis r10, storeR4@ha
stw r4, storeR4@l(r10)
lis r10, storeR31@ha
stw r31, storeR31@l(r10)

lis r10, storeF0@ha
stfs f0, storeF0@l(r10)
lis r10, storeF1@ha
stfs f1, storeF1@l(r10)
lis r10, storeF2@ha
stfs f2, storeF2@l(r10)
lis r10, storeF3@ha
stfs f3, storeF3@l(r10)
lis r10, storeF4@ha
stfs f4, storeF4@l(r10)
lis r10, storeF5@ha
stfs f5, storeF5@l(r10)
lis r10, storeF6@ha
stfs f6, storeF6@l(r10)
lis r10, storeF7@ha
stfs f7, storeF7@l(r10)
lis r10, storeF8@ha
stfs f8, storeF8@l(r10)
lis r10, storeF9@ha
stfs f9, storeF9@l(r10)
lis r10, storeF10@ha
stfs f10, storeF10@l(r10)
lis r10, storeF11@ha
stfs f11, storeF11@l(r10)
lis r10, storeF12@ha
stfs f12, storeF12@l(r10)
lis r10, storeF13@ha
stfs f13, storeF13@l(r10)

bla calculateNewRotation

lis r10, storeLR@ha
lwz r7, storeLR@l(r10)
mtlr r7

lis r10, storeR1@ha
lwz r1, storeR1@l(r10)
lis r10, storeR2@ha
lwz r2, storeR2@l(r10)
lis r10, storeR3@ha
lwz r3, storeR3@l(r10)
lis r10, storeR4@ha
lwz r4, storeR4@l(r10)
lis r10, storeR31@ha
lwz r31, storeR31@l(r10)

lis r10, storeF0@ha
lfs f0, storeF0@l(r10)
lis r10, storeF1@ha
lfs f1, storeF1@l(r10)
lis r10, storeF2@ha
lfs f2, storeF2@l(r10)
lis r10, storeF3@ha
lfs f3, storeF3@l(r10)
lis r10, storeF4@ha
lfs f4, storeF4@l(r10)
lis r10, storeF5@ha
lfs f5, storeF5@l(r10)
lis r10, storeF6@ha
lfs f6, storeF6@l(r10)
lis r10, storeF7@ha
lfs f7, storeF7@l(r10)
lis r10, storeF8@ha
lfs f8, storeF8@l(r10)
lis r10, storeF9@ha
lfs f9, storeF9@l(r10)
lis r10, storeF10@ha
lfs f10, storeF10@l(r10)
lis r10, storeF11@ha
lfs f11, storeF11@l(r10)
lis r10, storeF12@ha
lfs f12, storeF12@l(r10)
lis r10, storeF13@ha
lfs f13, storeF13@l(r10)

blr