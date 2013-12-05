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

#include "bcf_ordered_reader.h"

BCFOrderedReader::BCFOrderedReader(std::string _vcf_file, std::vector<GenomeInterval>& _intervals)
{
    ftype = hts_file_type(_vcf_file.c_str());
    if (!(ftype & (FT_VCF|FT_BCF|FT_STDIN)) )
    {
        fprintf(stderr, "[%s:%d %s] Not a VCF/BCF file: %s\n", __FILE__, __LINE__, __FUNCTION__, vcf_file.c_str());
        exit(1);
    }

    vcf_file = _vcf_file;
    intervals = _intervals;
    interval_index = 0;

    vcf = NULL;
    hdr = NULL;
    idx = NULL;
    tbx = NULL;
    itr = NULL;

    s = {0, 0, 0};
    vcf = bcf_open(vcf_file.c_str(), "r");
    hdr = bcf_alt_hdr_read(vcf);
    
    intervals_present =  intervals.size()!=0;

    if (ftype==FT_BCF_GZ)
    {
        if ((idx = bcf_index_load(vcf_file.c_str())))
        {
            index_loaded = true;
            //fprintf(stderr, "[I:%s] index loaded for %s\n", __FUNCTION__, vcf_file.c_str());
        }
        else
        {
            //fprintf(stderr, "[W:%s] index not loaded for %s\n", __FUNCTION__, vcf_file.c_str());
        }
    }
    else if (ftype==FT_VCF_GZ)
    {
        if ((tbx = tbx_index_load(vcf_file.c_str())))
        {
            index_loaded = true;
            //fprintf(stderr, "[I:%s] index loaded for %s\n", __FUNCTION__, vcf_file.c_str());
        }
        else
        {
            //fprintf(stderr, "[W:%s] index not loaded for %s\n", __FUNCTION__, vcf_file.c_str());
        }
    }

    random_access_enabled = intervals_present && index_loaded;
};

/**
 * Gets sequence name of a record
 */
const char* BCFOrderedReader::get_seqname(bcf1_t *v)
{
    return bcf_get_chrom(hdr, v);
};

/**
 * Gets bcf header.
 */
bcf_hdr_t* BCFOrderedReader::get_hdr()
{
    return hdr;
};

/**
 * Initialize next interval.
 * Returns false only if all intervals are accessed.
 */
bool BCFOrderedReader::initialize_next_interval()
{
    while (interval_index!=intervals.size())
    {
        if (ftype==FT_BCF_GZ)
        {
            //todo: move to querys
            GenomeInterval interval = intervals[interval_index];
            int tid = bcf_hdr_name2id(hdr, interval.seq.c_str());
            itr = bcf_itr_queryi(idx, tid, interval.start1-1, interval.end1);
            ++interval_index;
            if (itr)
            {
                return true;
            }
        }
        else if (ftype==FT_VCF_GZ)
        {
            intervals[interval_index++].to_string(&s);
            itr = tbx_itr_querys(tbx, s.s);
            if (itr)
            {
                return true;
            }
        }
    }

    return false;
};

/**
 * Reads next record, hides the random access of different regions from the user.
 */
bool BCFOrderedReader::read(bcf1_t *v)
{
    if (random_access_enabled)
    {
        if (ftype == FT_BCF_GZ)
        {
            while(true)
            {
                if (itr && bcf_itr_next(vcf, itr, v)>=0)
                {
                    return true;
                }
                else if (!initialize_next_interval())
                {
                    return false;
                }
            }
        }
        else
        {
            while(true)
            {
                if (itr && tbx_itr_next(vcf, tbx, itr, &s)>=0)
                {
                    vcf_parse1(&s, hdr, v);
                    return true;
                }
                else if (!initialize_next_interval())
                {
                    return false;
                }
            }
        }
    }
    else
    {
        if (bcf_read(vcf, hdr, v)==0)
        {
            //todo: filter via interval tree
            //if found in tree, return true else false
            return true;
        }
        else
        {
            return false;
        }
    }

    return false;
};

/**
 * Returns next set of vcf records at a start position.
 */
bool BCFOrderedReader::read_next_position(std::vector<bcf1_t *>& vs)
{
    //put records in pool
    for (uint32_t i=0; i<vs.size(); ++i)
    {
       store_bcf1_into_pool(vs[i]);
    }
    vs.clear();

    while (read(v))
    {
        if (true)
        {
        }
    }
    
    return false;
};

/**
 * Returns record to pool
 */
void BCFOrderedReader::store_bcf1_into_pool(bcf1_t* v)
{
    bcf_clear(v);
    pool.push_back(v);
}

/**
 * Gets record from pool, creates a new record if necessary
 */
bcf1_t* BCFOrderedReader::get_bcf1_from_pool()
{
    if(!pool.empty())
    {
        bcf1_t* v = pool.front();
        pool.pop_front();
        return v;
    }
    else
    {
        return bcf_init1();
    }
};

/**
 * Closes the file.
 */
void BCFOrderedReader::close()
{   
    bcf_close(vcf);
}


