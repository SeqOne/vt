/* The MIT License

   Copyright (c) 2016 Adrian Tan <atks@umich.edu>

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
   THE SOFTWARE.
*/

#ifndef VNTR_GENOTYPING_RECORD_H
#define VNTR_GENOTYPING_RECORD_H

#include "htslib/vcf.h"
#include "htslib/faidx.h"
#include "bcf_ordered_writer.h"
#include "variant.h"
#include "hts_utils.h"
#include "augmented_bam_record.h"
#include "estimator.h"
#include "genotyping_record.h"

#define FILTER_MASK_OVERLAP_SNP   0x0001
#define FILTER_MASK_OVERLAP_INDEL 0x0002
#define FILTER_MASK_OVERLAP_VNTR  0x0004

/**
 * A generic record that holds information for genotyping a
 * variant across multiple samples.
 *
 * Maintains read information and allows for additional reads
 * till VCF record can be printed out.
 */
class VNTRGenotypingRecord : GenotypingRecord
{
    public:

    //vntr specific record
    std::string motif;

    //vntr specific record
    //std::vector<float> counts;

    // sample level information
    int32_t nsamples;
    kstring_t alleles;
    std::vector<std::string> v_alleles;
    uint32_t n_filter;

    uint8_t* pls;
    uint8_t* ads;
    Estimator* est;

    // sufficient statistics for computing INFO field
    float bqr_num, bqr_den;
    float mqr_num, mqr_den;
    float cyr_num, cyr_den;
    float str_num, str_den;
    float nmr_num, nmr_den;
    float ior_num, ior_den;
    float nm0_num, nm0_den;
    float nm1_num, nm1_den;
    float abe_num, abe_den;
    float abz_num, abz_den;
    float ns_nref, dp_sum, max_gq;

    int32_t tmp_dp_q20;
    int32_t tmp_dp_ra;
    int32_t tmp_bq_s1, tmp_bq_s2;
    int32_t tmp_mq_s1, tmp_mq_s2;
    float tmp_cy_s1, tmp_cy_s2;
    int32_t tmp_st_s1, tmp_st_s2;
    int32_t tmp_al_s1, tmp_bq_al, tmp_mq_al;
    float  tmp_cy_al;
    int32_t tmp_st_al, tmp_nm_al;
    int32_t tmp_nm_s1, tmp_nm_s2;
    double tmp_oth_exp_q20, tmp_oth_obs_q20;
    double tmp_pls[3];
    double tmp_ads[3];

    // temporary information to be cleared out per-sample basis

    /**
     * Constructor.
     * @v - VCF record.
     */
    VNTRGenotypingRecord() {};

    /**
     * Constructor.
     * @v - VCF record.
     */
    VNTRGenotypingRecord(bcf_hdr_t *h, bcf1_t *v, int32_t vtype, int32_t nsamples, int32_t ploidy, Estimator* est);

    /**
     * Clears this record.
     */
    void clear();
    
    /**
     * Clears the temporary variables in this record.
     */
    void clearTemp();
        
    /**
     * Prints out variant with collected features and genotype information to a bcf object.
     */
    bcf1_t* flush_variant(bcf_hdr_t* hdr);
    
    /**
     * Adds information from a sample to aggregated data.
     */
    void flush_sample(int32_t sampleIndex);
    
    /**
     * Adds an allele based with collected sufficient statistics.
     */
    void add_allele(double contam, int32_t allele, uint8_t mapq, bool fwd, uint32_t q, int32_t cycle, uint32_t nm);
    
    /**
     * Collects sufficient statistics from read for variants to be genotyped.
     */
    void process_read(AugmentedBAMRecord& as, int32_t sampleIndex, double contam);

    /**
     * Destructor.
     */
    ~VNTRGenotypingRecord();
};

#endif