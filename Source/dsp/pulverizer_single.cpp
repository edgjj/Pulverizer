/* Copyright 2021 Yegor Suslin
 *
 * This file is part of Pulverizer.
 *
 * Pulverizer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Pulverizer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Pulverizer.  If not, see <http://www.gnu.org/licenses/>.
 */


#include "pulverizer_single.hpp"


namespace kraps
{
namespace filters
{
PulverizerSingle::PulverizerSingle() : Processor (p_pulverizer, 3, 1)
{
	pt = kraps::parameter::pt::ParameterTable(
		{ new parameter::Parameter<float>("frequency", 400, 400, 30, 20000),
		 new parameter::Parameter<float>("res", 0.5, 0.5, 0.5, 16),
		 new parameter::Parameter<int>("amount", 4, 4, 0, 100),
		 new parameter::Parameter<bool>("is_x2", false, false, false, true)
		});

}
PulverizerSingle::~PulverizerSingle()
{

}

void PulverizerSingle::recalculate_sr()
{
	precalc_sr = 2 * M_PI / sample_rate;
	calc_cores();
}

float8 PulverizerSingle::ssin(const float8& v)
{
	return (float8(16) * v * (float8(M_PI) - v))
		/ (float8(5 * M_PI * M_PI) - float8(4) * v * (float8(M_PI) - v));

}

void PulverizerSingle::calc_cores()
{
	float8 omega_0 = freq * precalc_sr;
	float8 alpha = ssin(omega_0) / ( float8 (2) * q);

	_c.a0 = float8(1) + alpha;
	_c.b0 = ( float8(1) - alpha ) / _c.a0;
	_c.b1 = ( float8 (-2) * ssin( float8 (M_PI_2) - omega_0) ) / _c.a0;
	_c.b2 = 1;
	_c.a1 = _c.b1;
	_c.a2 = _c.b0;

}

void PulverizerSingle::process_params()
{
	float8 cst = 20000;

	freq = pt.get_raw_value("frequency");
	q = pt.get_raw_value("res");
	amt = pt.get_raw_value("amount");
	is_x2 = pt.get_raw_value("is_x2");


	calc_cores();
}

float8 PulverizerSingle::process_stage(const float8& in, const bool& is_second = true, const int& num_stage = 0)
{

	std::array <BiquadDF1UnitD, 100>& _cores = is_second == true ? cores_2 : cores_1;


	float8 ret = in * _c.b0 + _cores[num_stage].z1[0] * _c.b1 + _cores[num_stage].z2[0] * _c.b2
							- _cores[num_stage].z1[1] * _c.a1 - _cores[num_stage].z2[1] * _c.a2;

	_cores[num_stage].z2[0] = _cores[num_stage].z1[0];
	_cores[num_stage].z2[1] = _cores[num_stage].z1[1];
	_cores[num_stage].z1[0] = in;
	_cores[num_stage].z1[1] = ret;

	return ret;

}

void PulverizerSingle::process_callback()
{

	float8 out = *inputs[kPulverizerAudioIn];

	for (int j = 0; j < amt; j++)
		out = process_stage( out, false, j);

	if (is_x2)
		for (int j = 0; j < amt; j++)
			out = process_stage( out, true, j);


	*outputs[kPulverizerAudioOut] = out;


}




}
}
