/* The MIT License

   Copyright (c) 2013 Adrian Tan <atks@umich.edu>

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

#ifndef VARIANT_MANIP_H
#define VARIANT_MANIP_H

#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <cmath>
#include <cfloat>
#include <vector>
#include <map>
#include <queue>
#include <list>
#include <sstream>
#include "htslib/faidx.h"
#include "htslib/vcf.h"
#include "htslib/kseq.h"
#include "hts_utils.h"

#define VT_REF   0
#define VT_SNP   1
#define VT_MNP   2
#define VT_INSERTION  4
#define VT_DELETION   8
#define VT_INDEL (VT_INSERTION|VT_DELETION)
#define VT_CLUMP 64
#define VT_CLUMP 64
#define VT_COMPLEX (VT_SNP|VT_INSERTION|VT_DELETION)

/**
 * Variant.
 */
class Variant
{
	public:
	    
	int32_t type;
	int32_t len;     //variant length
	kstring_t motif; //motif
    int32_t mlen;    //motif length
    int32_t tlen;    //reference tract length
    //std::vector 
        
    void clear()
    {
        type = 0;
        len = 0; 
    	motif.l = 0;
        mlen = 0;
        tlen = 0;
    }
    
    ~Variant()
    {
        if (motif.m)
        {
            free(motif.s);
        }    
    }
};

/**
 * Allele
 */
class Allele
{
	public:
	    
	int32_t type;
	int32_t len;  //variant length
	kstring_t motif;  //motif
    int32_t mlen; //motif length
    int32_t tlen; //reference tracct length

    void clear()
    {
        type = 0;
        len = 0; 
    	motif.l = 0;
        mlen = 0;
        tlen = 0;
    }
    
    ~Allele()
    {
        if (motif.m)
        {
            free(motif.s);
        }    
    }
};

/**
 * Methods for manipulating variants
 */
class VariantManip
{
    public:

    faidx_t *fai;

    VariantManip(std::string ref_fasta_file);

    /**
     * Detects near by STRs.
     */
    bool detect_str(const char* chrom, uint32_t pos1, std::string& motif, uint32_t& tlen);

    /**
     * Converts VTYPE to string.
     */
    std::string vtype2string(int32_t VTYPE);
    
    /**
     * Classifies variants.
     */
    int32_t classify_variant(const char* chrom, uint32_t pos1, char** allele, int32_t n_allele, Variant& v);
    
    /**
     * Left trims a variant with unnecesary nucleotides.
     */
    void left_trim(std::vector<std::string>& alleles, uint32_t& pos1, uint32_t& left_trimmed) ;

    /**
     * Left aligns a variant.
     */
    void left_align(std::vector<std::string>& alleles, uint32_t& pos1, const char* chrom, uint32_t& leftAligned, uint32_t& right_trimmed);

    /**
     * Generates a probing haplotype with flanks around the variant of interest.
     */
    void generate_probes(const char* chrom,
                        int32_t pos1, uint32_t probeDiff, 
                        std::vector<std::string>& alleles, //store alleles
                        std::vector<std::string>& probes, //store probes
                        uint32_t min_flank_length,
                        int32_t& preambleLength); //store preamble length

    private:

    /**
     * Recursive helper method for generateProbes.
     */
    void generate_probes(const char* chrom,
                        int32_t pos1,
                        uint32_t flankLength,
                        uint32_t& currentDiff,
                        uint32_t& length,
                        uint32_t gald,
                        std::vector<uint32_t>& diff,
                        std::vector<std::string>& alleles,
                        std::vector<std::string>& probes);

};

#endif
