OPTFLAG = -O3
INCLUDES = -I./lib -I. -I./lib/htslib -I./lib/Rmath -I./lib/pcre2
CXXFLAGS = -pipe -std=c++0x $(OPTFLAG) $(INCLUDES) -D__STDC_LIMIT_MACROS
CXX = g++

SOURCESONLY =

SOURCES = ahmm\
		align\
		allele\
		annotate_1000g\
		annotate_dbsnp_rsid\
		annotate_indels\
		annotate_indels2\
		annotate_regions\
		annotate_variants\
		annotate_vntrs\
		augmented_bam_record\
		bcf_genotyping_buffered_reader\
		bcf_single_genotyping_buffered_reader\
		bam_ordered_reader\
		bcf_ordered_reader\
		bcf_ordered_writer\
		bcf_synced_reader\
		bed\
		candidate_motif_picker\
		candidate_region_extractor\
		cat\
		chmm\
		complex_genotyping_record\
		compute_concordance\
		compute_features\
		compute_features2\
		compute_rl_dist\
		config\
		consolidate_multiallelics\
		consolidate_vntrs\
		consolidate\
		construct_probes\
		decompose\
		decompose2\
		decompose_blocksub\
		discover\
		duplicate\
		estimate\
		estimator\
		extract_vntrs\
		filter\
		filter_overlap\
		flank_detector\
		fuzzy_aligner\
		fuzzy_partition\
		gencode\
		genome_interval\
		genotype\
		genotyping_record\
		ghmm\
		hts_utils\
		hfilter\
		indel_annotator\
		indel_genotyping_record\
		index\
		info2tab\
		interval_tree\
		interval\
		lfhmm\
		lhmm\
		lhmm1\
		liftover\
		log_tool\
		merge\
		merge_candidate_variants\
		merge_genotypes\
		milk_filter\
		motif_tree\
		motif_map\
		multi_partition\
		multiallelics_consolidator\
		needle\
		normalize\
		nuclear_pedigree\
		ordered_bcf_overlap_matcher\
		ordered_region_overlap_matcher\
		partition\
		paste\
		paste_and_compute_features_sequential\
		paste_genotypes\
		pedigree\
		peek\
		pileup\
		pregex\
		profile_afs\
		profile_chm1\
		profile_chrom\
		profile_fic_hwe\
		profile_hwe\
		profile_indels\
		profile_len\
		profile_mendelian\
		profile_na12878\
		profile_snps\
		profile_vntrs\
		program\
		read_filter\
		reference_sequence\
		rfhmm\
		rfhmm_x\
		rminfo\
		seq\
		set_ref\
		snp_genotyping_record\
		sort\
		subset\
		sv_tree\
		svm_train\
		svm_predict\
		tbx_ordered_reader\
		test\
		trio\
		union_variants\
		uniq\
		utils\
		validate\
		variant\
		variant_manip\
		variant_filter\
		view\
		vntr\
		vntr_annotator\
		vntr_consolidator\
		vntr_extractor\
		vntr_genotyping_record\
		vntr_tree\
		vntrize\
		wdp_ahmm\

SOURCESONLY = main.cpp

TARGET = vt
TOOLSRC = $(SOURCES:=.cpp) $(SOURCESONLY)
TOOLOBJ = $(TOOLSRC:.cpp=.o)
LIBDEFLATE = lib/libdeflate/libdeflate.a
LIBHTS = lib/htslib/libhts.a
LIBRMATH = lib/Rmath/libRmath.a
LIBPCRE2 = lib/pcre2/libpcre2.a
LIBSVM = lib/libsvm/libsvm.a

all : $(TARGET)

${LIBDEFLATE} :
	cd lib/libdeflate; $(MAKE) || exit 1; 
	
${LIBHTS} : ${LIBDEFLATE}
	export LDFLAGS=-L${PWD}/lib/libdeflate;	export CPPFLAGS=-I${PWD}/lib/libdeflate; cd lib/htslib; autoheader; autoconf; ./configure; $(MAKE) libhts.a || exit 1; 

${LIBRMATH} :
	cd lib/Rmath; $(MAKE) libRmath.a || exit 1; 

${LIBPCRE2} :
	cd lib/pcre2; $(MAKE) libpcre2.a || exit 1; 

${LIBSVM} :
	cd lib/libsvm; $(MAKE) libsvm.a || exit 1; 

version :
	git rev-parse HEAD | cut -c 1-8 | awk '{print "#define VERSION \"0.5772-"$$0"\""}' > version.h;

$(TARGET) : ${LIBHTS} ${LIBRMATH} ${LIBPCRE2}  ${LIBSVM} $(TOOLOBJ) 
	$(CXX) $(CXXFLAGS) -o $@ $(TOOLOBJ) $(LIBHTS) $(LIBRMATH) ${LIBPCRE2} ${LIBDEFLATE} -lz -lpthread -lbz2 -llzma -lcurl -lcrypto

$(TOOLOBJ): $(HEADERSONLY)

.cpp.o :
	$(CXX) $(CXXFLAGS) -o $@ -c $*.cpp

.PHONY: clean cleanvt test version

clean :
	cd lib/libdeflate; $(MAKE) clean
	cd lib/htslib; $(MAKE) clean
	cd lib/Rmath; $(MAKE) clean
	cd lib/pcre2; $(MAKE) clean
	cd lib/libsvm; $(MAKE) clean
	-rm -rf $(TARGET) $(TOOLOBJ)

cleanvt :
	-rm -rf $(TARGET) $(TOOLOBJ)    

test : vt
	test/test.sh
	test/test_mnv.sh

debug : vt
	test/test.sh debug
