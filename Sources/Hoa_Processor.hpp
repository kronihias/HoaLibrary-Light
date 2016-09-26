/*
// Copyright (c) 2012-2016 CICM - Universite Paris 8 - Labex Arts H2H.
// Authors :
// 2012: Pierre Guillot, Eliott Paris & Julien Colafrancesco.
// 2012-2015: Pierre Guillot & Eliott Paris.
// 2015: Pierre Guillot & Eliott Paris & Thomas Le Meur (Light version)
// 2016: Pierre Guillot & Eliott Paris.
// For information on usage and redistribution, and for a DISCLAIMER OF ALL
// WARRANTIES, see the file, "LICENSE.txt," in this distribution.
*/

#ifndef DEF_HOA_PROCESSOR_LIGHT
#define DEF_HOA_PROCESSOR_LIGHT

#include "Hoa_Harmonics.hpp"
#include "Hoa_Planewaves.hpp"

namespace hoa
{
    //! @brief The processor.
    //! @details The processor the performs digital signal processing.
    template <Dimension D, typename T> class Processor
    {
    public:
        //! @brief The destructor.
        virtual ~Processor() hoa_noexcept hoa_default_f
        
        //! @brief The pure virtual method performs that performs the digital signal processing.
        //! @param inputs  The inputs array.
        //! @param outputs The outputs array.
        virtual void process(const T* input, T* outputs) hoa_noexcept = 0;

        class Planewaves;
    };

    //! @brief The harmonic processor.
    //! @details The harmonic processor owns a set of harmonics depending on the order of decomposition \f$N\f$.
    template <Dimension D, typename T> class ProcessorHarmonics : virtual public Processor<D, T>
    {
    public:

        //! @brief The harmonics constructor.
        //! @param order The order of decomposition \f$N\f$, must be at least 1.
        ProcessorHarmonics(const size_t order) hoa_noexcept :
        m_order_of_decomposition(order)
        {
            for(size_t i = 0; i < Harmonic<D, T>::getNumberOfHarmonics(order); ++i)
            {
                m_harmonics.push_back(Harmonic<D, T>(i));
            }
        }

        //! @brief The harmonics destructor.
        virtual ~ProcessorHarmonics() hoa_noexcept { m_harmonics.clear(); }

        //! @brief Returns the order of decomposition \f$N\f$.
        inline size_t getDecompositionOrder() const hoa_noexcept { return m_order_of_decomposition; }

        //! @brief Returns the number of harmonics.
        inline size_t getNumberOfHarmonics() const hoa_noexcept { return m_harmonics.size(); }

        //! @brief Returns the degree \f$l\f$ of an harmonic in the range \f$0\f$ to \f$N\f$.
        //! @param index The index of an harmonic.
        inline size_t getHarmonicDegree(const size_t index) const hoa_noexcept {
            return m_harmonics[index].getDegree(); }

        //! @brief Returns the azimuthal order \f$m\f of an harmonic in the range \f$-l\f$ to \f$l\f$.
        //! @param index The index of an harmonic.
        inline long getHarmonicOrder(const size_t index) const hoa_noexcept {
            return m_harmonics[index].getOrder(); }

        //! @brief Returns the index of an harmonic given the degree \f$l\f$ and the azimuthal order \f$m\f$ in the range \f$0\f$ to number of harmonics - 1.
        //! @param degree The degree of the harmonic.
        //! @param order  The azimuthal order of the harmonic.
        inline size_t getHarmonicIndex(const size_t degree, long order) const hoa_noexcept {
            return Harmonic<D, T>::getIndex(degree, order); }

        //! @brief Returns the name of an harmonic.
        //! @param index The index of an harmonic.
        inline std::string getHarmonicName(const size_t index) const hoa_noexcept {
            return m_harmonics[index].getName(); }
        
        //! @brief Returns the normalization of an harmonic.
        //! @param index The index of an harmonic.
        inline T getHarmonicNormalization(const size_t index) const hoa_noexcept {
            return m_harmonics[index].getNormalization(); }
        
        //! @brief Returns the semi-normalization of an harmonic.
        //! @param index The index of an harmonic.
        inline T getHarmonicSemiNormalization(const size_t index) const hoa_noexcept {
            return m_harmonics[index].getSemiNormalization(); }
        
    private:
        
        const size_t                  m_order_of_decomposition;
        std::vector< Harmonic<D, T> > m_harmonics;
    };


    //! The planewave processor.
    /** The planewave processor owns a set of planewaves.
     */
    template <Dimension D, typename T> class Processor<D, T>::Planewaves : virtual public Processor<D, T>
    {
    private:
        const size_t                 m_number_of_planewaves;
        std::vector<Planewave<D, T> >    m_planewaves;
        T                           m_rotation_z;
        T                           m_rotation_y;
        T                           m_rotation_x;

    public:

        //! The planewaves constructor.
        /** The planewaves constructor allocates and initializes the general member values depending on a number of planewaves. The number of planewaves must be a least 1.
         @param     numberOfPlanewaves	The number of planewaves.
         */
        Planewaves(const size_t numberOfPlanewaves) hoa_noexcept :
        m_number_of_planewaves(numberOfPlanewaves),
        m_rotation_z(0.),
        m_rotation_y(0.),
        m_rotation_x(0.)
        {
#ifndef DOXYGEN_SHOULD_SKIP_THIS
            if(D == Hoa2d)
            {
                for(size_t i = 0; i < m_number_of_planewaves; i++)
                {
                    m_planewaves.push_back(Planewave<D, T>(i+1, (T)i / (m_number_of_planewaves) * HOA_2PI, 0.));
                }
            }
            else
            {
                if(m_number_of_planewaves == 4)
                {
                    const double oh = -(sqrt(2. / 3.) / sqrt(3. / 8.) - 1.);
                    const double hc = sqrt(1. - oh * oh);
                    const double el = asin(oh / sqrt(hc*hc + oh*oh));
                    m_planewaves.push_back(Planewave<D, T>(1ul,  0., (T)HOA_PI2));
                    m_planewaves.push_back(Planewave<D, T>(2ul, 0., el));
                    m_planewaves.push_back(Planewave<D, T>(3ul, (T)(HOA_2PI / 3.), el));
                    m_planewaves.push_back(Planewave<D, T>(4ul, (T)(HOA_2PI / 1.5), el));
                }
                else if(m_number_of_planewaves == 6)
                {
					m_planewaves.push_back(Planewave<D, T>(1ul, 0., (T)HOA_PI2));
                    m_planewaves.push_back(Planewave<D, T>(2ul, 0., 0.));
					m_planewaves.push_back(Planewave<D, T>(3ul, (T)HOA_PI2, 0.));
					m_planewaves.push_back(Planewave<D, T>(4ul, (T)(2. * HOA_PI2), 0.));
					m_planewaves.push_back(Planewave<D, T>(5ul, (T)(3. * HOA_PI2), 0.));
					m_planewaves.push_back(Planewave<D, T>(6ul, 0., (T)-HOA_PI2));
                }
                else if(m_number_of_planewaves == 8)
                {
                    m_planewaves.push_back(Planewave<D, T>(1ul, 1., 1., 1.));
                    m_planewaves.push_back(Planewave<D, T>(2ul, -1., 1., 1.));
                    m_planewaves.push_back(Planewave<D, T>(3ul, -1., -1., 1.));
                    m_planewaves.push_back(Planewave<D, T>(4ul, 1., -1., 1.));

                    m_planewaves.push_back(Planewave<D, T>(5ul, 1., 1., -1.));
                    m_planewaves.push_back(Planewave<D, T>(6ul, -1., 1., -1.));
                    m_planewaves.push_back(Planewave<D, T>(7ul, -1., -1., -1.));
                    m_planewaves.push_back(Planewave<D, T>(8ul, 1., -1., -1.));
                }
                else if(m_number_of_planewaves == 12)
                {
					m_planewaves.push_back(Planewave<D, T>(1, 0., (T)HOA_PI2));
                    for(size_t i = 1; i < 6; i++)
                    {
                        m_planewaves.push_back(Planewave<D, T>(i*2, T(i - 1.) / 5. * HOA_2PI, atan(0.5)));
                        m_planewaves.push_back(Planewave<D, T>(i*2+1, T(i - 1.) / 5. * HOA_2PI - HOA_PI / 5., -atan(0.5)));
                    }
					m_planewaves.push_back(Planewave<D, T>(12, 0., (T)-HOA_PI2));
                }
                else if(m_number_of_planewaves == 20)
                {
                    const T phi = (sqrt(5.) - 1.) / 2.;
                    const T R = 1. / sqrt(3.);
                    const T a = R;
                    const T b = R / phi;
                    const T c = R * phi;
                    size_t index = 1;
                    for(long i = -1; i < 2; i += 2)
                    {
                        for(long j = -1; j < 2; j += 2)
                        {
                            m_planewaves.push_back(Planewave<D, T>(index++, 0., i * c * R, -j * b * R));
                            m_planewaves.push_back(Planewave<D, T>(index++, i * c * R, j * b * R, 0.));
                            m_planewaves.push_back(Planewave<D, T>(index++, i * b * R, 0., -j * c * R));
                            for(long k = -1; k < 2; k += 2)
                            {
                                m_planewaves.push_back(Planewave<D, T>(index++, i * a * R, j * a * R, -k * a * R));
                            }
                        }
                    }
                }
                else
                {
                    if(m_number_of_planewaves % 2)
                    {
						m_planewaves.push_back(Planewave<D, T>(1, 0., (T)HOA_PI2));
                    }
                    const T     phi     = (sqrt(5.) - 1.) / 4.;
                    const size_t limit   = (m_number_of_planewaves - (m_number_of_planewaves % 2)) / 2;
                    const T     offset  = 1. / T(limit) * HOA_PI;
                    for(size_t i = 0; i < m_number_of_planewaves - (m_number_of_planewaves % 2); i++)
                    {
                        if(i < limit)
                        {
                            m_planewaves.push_back(Planewave<D, T>(i+1, T(i) / T(limit) * HOA_2PI, phi));
                        }
                        else
                        {
                            m_planewaves.push_back(Planewave<D, T>(i+1, T(i - (limit)) / T(limit) * HOA_2PI + offset, phi * 2));
                        }
                    }
                }
            }
#endif
        }

        //! The planewaves destructor.
        /** The planewaves destructor.
         */
        virtual ~Planewaves() hoa_noexcept
        {
            m_planewaves.clear();
        }

        //! Retrieve the order of decomposition.
        /** Retrieve the order of decomposition.
         @return The order.
         */
        inline size_t getNumberOfPlanewaves() const hoa_noexcept
        {
            return m_number_of_planewaves;
        }

        //! Set the offset of the planewaves.
        /**	Set the azimuth offset of the planewaves in radian.
         @param     x_axe		An azimuth value.
         @param     y_axe		An azimuth value.
         @param     z_axe		An azimuth value.
         */
        inline void setPlanewavesRotation(const T x_axe, const T y_axe, const T z_axe) hoa_noexcept
        {
            m_rotation_x = Math<T>::wrap_twopi(x_axe);
            m_rotation_y = Math<T>::wrap_twopi(y_axe);
            m_rotation_z = Math<T>::wrap_twopi(z_axe);
        }

        //! Get the offset of the planewaves.
        /**	Retrieve the azimuth offset of the planewaves in radian.
         @return    The offset of the planewaves.
         */
        inline T getPlanewavesRotationX() const hoa_noexcept
        {
            return m_rotation_x;
        }

        //! Get the offset of the planewaves.
        /**	Retrieve the azimuth offset of the planewaves in radian.
         @return    The offset of the planewaves.
         */
        inline T getPlanewavesRotationY() const hoa_noexcept
        {
            return m_rotation_y;
        }

        //! Get the offset of the planewaves.
        /**	Retrieve the azimuth offset of the planewaves in radian.
         @return    The offset of the planewaves.
         */
        inline T getPlanewavesRotationZ() const hoa_noexcept
        {
            return m_rotation_z;
        }

        //! Get the index of a planewave.
        /** Get the index of a planewave.
         @param      index   The index of the planewave.
         @return     The index of the planewave.
         */
        inline size_t getPlanewaveIndex(const size_t index) hoa_noexcept
        {
            return m_planewaves[index].getIndex();
        }

        //! Set the azimuth of a planewave.
        /** Set the azimuth of a planewave. The azimuth of the planewaves is in radian, 0 radian is at the front of the soundfield and π is at the back of the sound field. The maximum index must be the number of planewaves - 1.

         @param     index   The index of the planewave.
         @param     azimuth The new azimuth of the planewave.
         */
        inline void setPlanewaveAzimuth(const size_t index, const T azimuth) hoa_noexcept
        {
            m_planewaves[index].setAzimuth(Math<T>::wrap_twopi(azimuth));
        }

        //! Get the azimuth of a planewave.
        /** Get the azimuth of a planewave. The azimuth of the planewaves is in radian, 0 radian is at the front of the soundfield and π is at the back of the sound field. The maximum index must be the number of planewaves - 1.

         @param      index   The index of the planewave.
         @param      rotation   False if you don't want to consider the rotation, otherwise true (default).
         @return     The azimuth of the planewave.
         */
        inline T getPlanewaveAzimuth(const size_t index, const bool rotation = true) const hoa_noexcept
        {
            return m_planewaves[index].getAzimuth(rotation ? m_rotation_x : T(0.), rotation ? m_rotation_y : T(0.), rotation ? m_rotation_z : T(0.));
        }

        //! Set the elevation of a planewave.
        /** Set the elevation of a planewave. The elevation of the planewaves is in radian. The maximum index must be the number of planewaves - 1.

         @param      index    The index of the planewave.
         @param      azimuth  The azimuth of the planewave.
         */
        inline void setPlanewaveElevation(const size_t index, const T azimuth) hoa_noexcept
        {
            m_planewaves[index].setElevation(Math<T>::wrap_pi(azimuth));
        }

        //! Get the elevation of a planewave.
        /** Get the elevation of a planewave. The elevation of the planewaves is in radian. The maximum index must be the number of planewaves - 1.

         @param      index      The index of the planewave.
         @param      rotation   False if you don't want to consider the rotation, otherwise true (default).
         @return     The elevation of the planewave.
         */
        inline T getPlanewaveElevation(const size_t index, const bool rotation = true) const hoa_noexcept
        {
            return m_planewaves[index].getElevation(rotation ? m_rotation_x : T(0.), rotation ? m_rotation_y : T(0.), rotation ? m_rotation_z : T(0.));
        }

        //! Get the abscissa of a planewave.
        /** Get the abscissa of a planewave. The abscissa is between -1 and 1, -1 is the left of the soundfield, 0 is the center of the soundfield and 1 is the right of the soundfield. The maximum index must be the number of planewaves - 1.
         @param     index    The index of the planewave.
         @param      rotation   False if you don't want to consider the rotation, otherwise true (default).
         @return    The abscissa of the planewave.
         */
        inline T getPlanewaveAbscissa(const size_t index, const bool rotation = true) const hoa_noexcept
        {
            return m_planewaves[index].getAbscissa(rotation ? m_rotation_x : T(0.), rotation ? m_rotation_y : T(0.), rotation ? m_rotation_z : T(0.));
        }

        //! Get the ordinate of a planewave.
        /** Get the ordinate of a planewave. The ordinate is between -1 and 1, -1 is the back of the soundfield, 0 is the center of the soundfield and 1 is the front of the soundfield. The maximum index must be the number of planewaves - 1.
         @param     index	The index of the planewave.
         @param      rotation   False if you don't want to consider the rotation, otherwise true (default).
         @return    The ordinate of the planewave.
         */
        inline T getPlanewaveOrdinate(const size_t index, const bool rotation = true) const hoa_noexcept
        {
            return m_planewaves[index].getOrdinate(rotation ? m_rotation_x : T(0.), rotation ? m_rotation_y : T(0.), rotation ? m_rotation_z : T(0.));
        }

        //! Get the height of a planewave.
        /** Get the ordinate of a planewave. The height is between -1 and 1, -1 is the back of the soundfield, 0 is the center of the soundfield and 1 is the front of the soundfield. The maximum index must be the number of planewaves - 1.
         @param     index	The index of the planewave.
         @param      rotation   False if you don't want to consider the rotation, otherwise true (default).
         @return    The height of the planewave.
         */
        inline T getPlanewaveHeight(const size_t index, const bool rotation = true) const hoa_noexcept
        {
            return m_planewaves[index].getHeight(rotation ? m_rotation_x : T(0.), rotation ? m_rotation_y : T(0.), rotation ? m_rotation_z : T(0.));
        }

        //! Get a name for a planewave.
        /** Get a name for a planewave in a std::string format that will be "Planewave index azimuth (in degrees)".
         @param     index	The index of a planewave.
         @return    The method returns a name for the planewave.
         */
        inline std::string getPlanewaveName(const size_t index) const hoa_noexcept
        {
            return m_planewaves[index].getName();
        }

        //! This method performs the processing.
        /**	You should use this method for in-place or not-in-place processing and sample by sample. The input array and the outputs array depends of the template and the processing.
         @param     input  The input array.
         @param     outputs The outputs array.
         */
        virtual void process(const T* input, T* outputs) hoa_noexcept
        {
            ;
        }
    };
}

#endif


