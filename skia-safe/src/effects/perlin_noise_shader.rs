use crate::core::{scalar, ISize, Shader};
use crate::prelude::*;
use skia_bindings::{
    C_SkPerlinNoiseShader_MakeFractalNoise, C_SkPerlinNoiseShader_MakeImprovedNoise,
    C_SkPerlinNoiseShader_MakeTurbulence,
};

pub enum PerlinNoiseShader {}

impl PerlinNoiseShader {
    pub fn fractal_noise<TS: Into<Option<ISize>>>(
        base_frequency: (scalar, scalar),
        num_octaves: i32,
        seed: scalar,
        tile_size: TS,
    ) -> Option<Shader> {
        Shader::from_ptr(unsafe {
            C_SkPerlinNoiseShader_MakeFractalNoise(
                base_frequency.0,
                base_frequency.1,
                num_octaves,
                seed,
                tile_size.into().native().as_ptr_or_null(),
            )
        })
    }

    pub fn turbulence<TS: Into<Option<ISize>>>(
        base_frequency: (scalar, scalar),
        num_octaves: i32,
        seed: scalar,
        tile_size: TS,
    ) -> Option<Shader> {
        Shader::from_ptr(unsafe {
            C_SkPerlinNoiseShader_MakeTurbulence(
                base_frequency.0,
                base_frequency.1,
                num_octaves,
                seed,
                tile_size.into().native().as_ptr_or_null(),
            )
        })
    }

    pub fn improved_noise(
        base_frequency: (scalar, scalar),
        num_octaves: i32,
        z: scalar,
    ) -> Option<Shader> {
        Shader::from_ptr(unsafe {
            C_SkPerlinNoiseShader_MakeImprovedNoise(
                base_frequency.0,
                base_frequency.1,
                num_octaves,
                z,
            )
        })
    }
}