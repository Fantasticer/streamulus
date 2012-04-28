//
//  strop.h
//
// Streamulus Copyright (c) 2012 Irit Katriel. All rights reserved.
//
// This file is part of Streamulus.
// 
// Streamulus is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Streamulus is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with Streamulus.  If not, see <http://www.gnu.org/licenses/>.
//

#pragma once

#include <boost/function_types/parameter_types.hpp>
#include <boost/function_types/result_type.hpp>
#include <boost/fusion/include/vector.hpp>
#include <boost/proto/proto.hpp>

#include "stream.h" 
#include "engine_api.h" 
#include "strop_base.h" 

namespace streamulus
{
    
    template<typename F> // F = function signature
    class Strop 
        : public StropBase
    {
    public:
        
        virtual ~Strop() 
        {
        }
        
        typedef typename boost::function_types::result_type<F>::type result_type;
        typedef boost::function_types::parameter_types<F> param_types;
        
        typedef boost::shared_ptr<Strop<F> > StropPtrType;
        
        virtual bool Compute(result_type& result)=0;
        
        // return true if you put any data on output streams, false otherwise
        virtual bool Work()
        {            
            result_type res;
            if (! Compute(res))
                return false;
            
            // std::cout << "Compute[" << mTopSortIndex << "]: " << res << std::endl;
            
            Output(res);
            return true;
        }
        
        void Output(const result_type& value)
        {
            mEngine->Output<result_type>(mVertexDescriptor,value);
        }
        
        template<typename Inputs> // a fusion sequence of the inputs
        void SetInputs(const Inputs& inputs)
        {
            mInputs=inputs;
        }
        
        template<typename Tinput, int I>
        boost::shared_ptr<Stream<Tinput> > Input()
        {
            return boost::fusion::at_c<I>(mInputs);
        }
        
        typedef boost::shared_ptr<Stream<result_type> > OutputStreamPtr;
        
        OutputStreamPtr MakeOutputStream() const
        {
            return OutputStreamPtr(new Stream<result_type>());
        }
                        
    private:
        template<typename T>
        struct MakeStreamPtrType 
        {
            typedef boost::shared_ptr<Stream<T>  > type;
        };
        typedef typename boost::mpl::transform<param_types,MakeStreamPtrType<boost::mpl::_1> >::type input_types;
        typename boost::fusion::result_of::as_vector<typename input_types::type>::type mInputs;
    };
    
    
} // ns streamulus