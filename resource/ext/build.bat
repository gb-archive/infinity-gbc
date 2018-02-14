@echo off

gbr2bin arrow_w.gbr arrow_w.bin /nopal 0 15
gbr2bin cure1.gbr cure1.bin /nopal 0 15
gbr2bin fire1.gbr fire1.bin /nopal 0 15
gbr2bin fx1.gbr fx1.bin /nopal 0 11
gbr2bin gems.gbr gemgfx.bin 0 15
gbr2bin items.gbr itemgfx.bin 0 11
gbr2bin jolt1.gbr jolt1.bin /nopal 0 4
gbr2bin menugfx.gbr menugfx.bin /nopal 0 95
gbr2bin menuwing.gbr menuwing.bin /nopal 0 1
gbr2bin miss.gbr missgfx.bin /nopal 0 3
gbr2bin poof.gbr poof.bin /nopal 0 3
gbr2bin scratch1.gbr scratch1.bin /nopal 0 15
gbr2bin skills.gbr skillgfx.bin 0 19
gbr2bin slash.gbr slash1.bin /nopal 0 3
gbr2bin slurp.gbr slurp.bin /nopal 0 15
gbr2bin e_roots2.gbr spawn1.bin /nopal 3 7
gbr2bin staff1.gbr staff1.bin /nopal 0 0
gbr2bin sword1.gbr sword1.bin /nopal 0 15
gbr2bin tidal1.gbr tidal1.bin /nopal 0 3
gbr2bin wake.gbr wake.bin /nopal 0 15
gbr2bin whirl1_adjusted.gbr whirl1.bin /nopal 0 11
gbr2bin wingding.gbr wingding.bin /nopal 0 23

if not exist ..\..\source\data mkdir ..\..\source\data

gfx2h sprite.pal ../../source/data/dudepal.h dudepal
gfx2h sprite_gba.pal ../../source/data/dudepal2.h dudepal2

gfx2h arrow_w.bin ../../source/data/arrow_w.h arrow_w
gfx2h cure1.bin ../../source/data/cure1.h cure1
gfx2h fire1.bin ../../source/data/fire1.h fire1
gfx2h fx1.bin ../../source/data/fx1.h fx1
gfx2h gemgfx.bin ../../source/data/gemgfx.h gemgfx
gfx2h itemgfx.bin ../../source/data/itemgfx.h itemgfx
gfx2h jolt1.bin ../../source/data/jolt1.h jolt1
gfx2h menugfx.bin ../../source/data/menugfx.h menugfx
gfx2h menuwing.bin ../../source/data/menuwing.h menuwing
gfx2h missgfx.bin ../../source/data/missgfx.h missgfx
gfx2h poof.bin ../../source/data/poof.h poofgfx
gfx2h scratch1.bin ../../source/data/scratch1.h scratch1
gfx2h skillgfx.bin ../../source/data/skillgfx.h skillgfx
gfx2h slash1.bin ../../source/data/slash1.h slash1
gfx2h slurp.bin ../../source/data/slurp.h slurpgfx
gfx2h spawn1.bin ../../source/data/spawn1.h spawn1
gfx2h staff1.bin ../../source/data/staff1.h staff1gfx
gfx2h sword1.bin ../../source/data/sword1.h sword1
gfx2h tidal1.bin ../../source/data/tidal1.h tidal1
gfx2h wake.bin ../../source/data/wake.h wakegfx
gfx2h whirl1.bin ../../source/data/whirl1.h whirl1
gfx2h wingding.bin ../../source/data/wingding.h wingding

gbr2bin advisor.gbr advisor.bin 0 7
gbr2bin aldric.gbr aldric.bin 0 7
gbr2bin alutha.gbr alutha.bin 0 63
gbr2bin anna.gbr anna.bin 0 7
gbr2bin archmagi.gbr archmagi.bin 0 7
gbr2bin barrier.gbr barrier.bin 0 7
gbr2bin boat.gbr boat.bin 0 7
gbr2bin bossomeg.gbr bossomeg.bin 0 35
gbr2bin connor.gbr conner.bin 0 63
gbr2bin deadguy.gbr deadguy.bin 0 7
gbr2bin e_archer.gbr e_archer.bin 0 8
gbr2bin e_armora.gbr e_armora.bin 0 8
gbr2bin e_bat2.gbr e_bat2.bin 0 8
gbr2bin e_bat3.gbr e_bat3.bin 0 8
gbr2bin e_bat.gbr e_bat.bin 0 8
gbr2bin ebexanim.gbr ebexanim.bin 0 35
gbr2bin e_blank.gbr e_blank.bin 0 8
gbr2bin e_bsaru.gbr e_bsaru.bin 0 8
gbr2bin e_cactrs.gbr e_cactrs.bin 0 8
gbr2bin e_dryath.gbr e_dryath.bin 0 8
gbr2bin e_durang.gbr e_durang.bin 0 8
gbr2bin e_exanim.gbr e_exanim.bin 0 8
gbr2bin e_eye.gbr e_eye.bin 0 8
gbr2bin e_fbat.gbr e_fbat.bin 0 8
gbr2bin e_flame.gbr e_flame.bin 0 8
gbr2bin e_folcon.gbr e_folcon.bin 0 8
gbr2bin e_fsold.gbr e_fsold.bin 0 8
gbr2bin e_fwerdg.gbr e_fwerdg.bin 0 8
gbr2bin e_gmask.gbr e_gmask.bin 0 8
gbr2bin e_golem2.gbr e_golem2.bin 0 8
gbr2bin e_golem3.gbr e_golem3.bin 0 8
gbr2bin e_golem4.gbr e_golem4.bin 0 8
gbr2bin e_golem.gbr e_golem.bin 0 8
gbr2bin e_grissl.gbr e_grissl.bin 0 8
gbr2bin e_grossl.gbr e_grossl.bin 0 8
gbr2bin e_grumb2.gbr e_grumb2.bin 0 8
gbr2bin e_grumble.gbr e_grumble.bin 0 8
gbr2bin e_gutana.gbr e_gutana.bin 0 8
gbr2bin e_hench.gbr e_hench.bin 0 8
gbr2bin e_herrera.gbr e_herrer.bin 0 8
gbr2bin e_horf.gbr e_horf.bin 0 8
gbr2bin e_iwamp2.gbr e_iwamp2.bin 0 8
gbr2bin e_iwamp.gbr e_iwamp.bin 0 8
gbr2bin e_leshiz.gbr e_leshiz.bin 0 8
gbr2bin e_lich.gbr e_lich.bin 0 8
gbr2bin elya.gbr elya.bin 0 63
gbr2bin e_masket.gbr e_masket.bin 0 8
gbr2bin embrace.gbr embrace.bin 0 7
gbr2bin e_mermod.gbr e_mermod.bin 0 8
gbr2bin e_mermud.gbr e_mermud.bin 0 8
gbr2bin e_mummy.gbr e_mummy.bin 0 8
gbr2bin e_mushro.gbr e_mushro.bin 0 8
gbr2bin e_nitsuj.gbr e_nitsuj.bin 0 8
gbr2bin en_tw1.gbr en_tw1.bin 0 8
gbr2bin en_tw2.gbr en_tw2.bin 0 8
gbr2bin en_tw3.gbr en_tw3.bin 0 8
gbr2bin e_oaff2.gbr e_oaff2.bin 0 8
gbr2bin e_oaff.gbr e_oaff.bin 0 8
gbr2bin e_omega.gbr e_omega.bin 0 8
gbr2bin e_ottor.gbr e_ottor.bin 0 8
gbr2bin e_pirate.gbr e_pirate.bin 0 8
gbr2bin e_ragnop.gbr e_ragnop.bin 0 8
gbr2bin e_rat.gbr e_rat.bin 0 8
gbr2bin e_roland.gbr e_roland.bin 0 8
gbr2bin e_roots2.gbr e_roots2.bin 0 8
gbr2bin e_roots.gbr e_roots.bin 0 8
gbr2bin e_saru.gbr e_saru.bin 0 8
gbr2bin e_sbat.gbr e_sbat.bin 0 8
gbr2bin e_sold.gbr e_sold.bin 0 8
gbr2bin e_spectr.gbr e_spectr.bin 0 8
gbr2bin e_sponge.gbr e_sponge.bin 0 8
gbr2bin e_ssaru.gbr e_ssaru.bin 0 8
gbr2bin e_swampy.gbr e_swampy.bin 0 8
gbr2bin e_swear.gbr e_swear.bin 0 8
gbr2bin e_syak.gbr e_syak.bin 0 8
gbr2bin e_tasm.gbr e_tasm.bin 0 8
gbr2bin e_tree1.gbr e_tree1.bin 0 8
gbr2bin etreebos.gbr etreebos.bin 0 35
gbr2bin e_victor.gbr e_victor.bin 0 8
gbr2bin e_waglo.gbr e_waglo.bin 0 8
gbr2bin e_wampy.gbr e_wampy.bin 0 8
gbr2bin e_wataku.gbr e_wataku.bin 0 8
gbr2bin e_watazk.gbr e_watazk.bin 0 8
gbr2bin e_wazzip.gbr e_wazzip.bin 0 8
gbr2bin e_werdg2.gbr e_werdg2.bin 0 8
gbr2bin e_werdge.gbr e_werdge.bin 0 8
gbr2bin e_witzki.gbr e_witzki.bin 0 8
gbr2bin ewolfbos.gbr ewolfbos.bin 0 35
gbr2bin e_wulfa.gbr e_wulfa.bin 0 8
gbr2bin e_yak.gbr e_yak.bin 0 8
gbr2bin e_yik.gbr e_yik.bin 0 8
gbr2bin e_yok.gbr e_yok.bin 0 8
gbr2bin e_yuk.gbr e_yuk.bin 0 8
gbr2bin e_zombie.gbr e_zombie.bin 0 8
gbr2bin fire2.gbr fire2.bin 0 7
gbr2bin fire3.gbr fire3.bin 0 7
gbr2bin fire4.gbr fire4.bin 0 7
gbr2bin fire5.gbr fire5.bin 0 7
gbr2bin fire6.gbr fire6.bin 0 7
gbr2bin fire7.gbr fire7.bin 0 7
gbr2bin fire8.gbr fire8.bin 0 7
gbr2bin geist.gbr geist.bin 0 7
gbr2bin gilus.gbr gilus.bin 0 7
gbr2bin golem.gbr golem.bin 0 7
gbr2bin hassan.gbr hassan.bin 0 7
gbr2bin longago.gbr head1.bin 0 7
gbr2bin longago.gbr head2.bin 4 11
gbr2bin longago.gbr head3.bin 12 19
gbr2bin izaur.gbr izuar.bin 0 1
gbr2bin kid1.gbr kid1.bin 0 7
gbr2bin kid2.gbr kid2.bin 0 7
gbr2bin kid3.gbr kid3.bin 0 7
gbr2bin kid4.gbr kid4.bin 0 7
gbr2bin kidhero.gbr kidhero.bin 0 7
gbr2bin kidmagi.gbr kidmagi.bin 0 7
gbr2bin kiddragon.gbr kidragon.bin 0 7
gbr2bin krait.gbr krait.bin 0 7
gbr2bin kraith2.gbr kraith2.bin 0 7
gbr2bin lady1.gbr lady1.bin 0 7
gbr2bin lady2.gbr lady2.bin 0 7
gbr2bin lady3.gbr lady3.bin 0 7
gbr2bin lady4.gbr lady4.bin 0 7
gbr2bin lady5.gbr lady5.bin 0 7
gbr2bin man1.gbr man1.bin 0 7
gbr2bin man2.gbr man2.bin 0 7
gbr2bin mayor.gbr mayor.bin 0 7
gbr2bin npcex1.gbr npcex1.bin 0 7
gbr2bin npc_exh.gbr npc_exh.bin 0 7
gbr2bin npcfire.gbr npcfire.bin 0 7
gbr2bin nselera.gbr nselera.bin 0 7
gbr2bin oldlady.gbr oldlady.bin 0 7
gbr2bin oldmagi.gbr oldmagi.bin 0 7
gbr2bin oldman.gbr oldman.bin 0 7
gbr2bin portal.gbr portala.bin 0 7
gbr2bin portal.gbr portalb.bin 8 15
gbr2bin roland.gbr roland.bin 0 63
gbr2bin ryan.gbr ryan.bin 0 63
gbr2bin smoke.gbr smoke.bin 0 7
gbr2bin e_sold.gbr soldier.bin 0 7
gbr2bin soulgem.gbr soulgem.bin 0 7
gbr2bin unnamed.gbr unnamed.bin 0 7
gbr2bin victor2.gbr victor2.bin 0 63
gbr2bin victor.gbr victor.bin 0 63
gbr2bin wake2.gbr wake2a.bin 0 7
gbr2bin wake2.gbr wake2b.bin 8 15
gbr2bin wolf.gbr wolf.bin 0 7
gbr2bin woomagi.gbr woomagi.bin 0 7

del game.j2w
j2w game.j2w infinity.in
j2w game.j2w
copy *.pag ..
