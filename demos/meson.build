ret2spec_sa = executable('ret2spec_sa', 'ret2spec_sa.c', dependencies: deps)
ret2spec_ca = executable('ret2spec_ca', 'ret2spec_ca.c', dependencies: deps)
spectre_pht_sa = executable('spectre_pht_sa', 'spectre_pht_sa.c', dependencies: deps)
spectre_btb_sa = executable('spectre_btb_sa', 'spectre_btb_sa.c', dependencies: deps)
spectre_btb_ca = executable('spectre_btb_ca', 'spectre_btb_ca.c', dependencies: deps)
if os == 'linux' and arch in ['x86_64', 'ppc64']
  l1tf = executable('l1tf', 'l1tf.c', dependencies: deps)
endif
