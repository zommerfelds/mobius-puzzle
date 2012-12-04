const float sigma = 6.0;

const float blurSize = 1.0 / 512.0; // This should usually be equal to
                                    // 1.0 / texture_pixel_width for a horizontal blur, and
                                    // 1.0 / texture_pixel_height for a vertical blur.
                         
uniform sampler2D blurSampler;  // Texture that will be blurred by this shader

const float pi = 3.14159265;


#if defined(BLUR_BIG)
const float numBlurPixelsPerSide = 20.0;
#elif defined(BLUR_SMALL)
const float numBlurPixelsPerSide = 5.0;
#else
#error "Missing define (1)"
#endif

#if defined(VERTICAL_BLUR)
const vec2  blurMultiplyVec      = vec2(0.0, 1.0);
#elif defined(HORIZONTAL_BLUR)
const vec2  blurMultiplyVec      = vec2(1.0, 0.0);
#else "Missing define (2)"
#endif

void main() {
	
  // Incremental Gaussian Coefficent Calculation (See GPU Gems 3 pp. 877 - 889)
  vec3 incrementalGaussian;
  incrementalGaussian.x = 1.0 / (sqrt(2.0 * pi) * sigma);
  incrementalGaussian.y = exp(-0.5 / (sigma * sigma));
  incrementalGaussian.z = incrementalGaussian.y * incrementalGaussian.y;

  float avgValue = 0.0;
  float coefficientSum = 0.0;

  // Take the central sample first...
  avgValue += texture2D(blurSampler, gl_TexCoord[0].xy).x * incrementalGaussian.x;
  coefficientSum += incrementalGaussian.x;
  incrementalGaussian.xy *= incrementalGaussian.yz;

  // Go through the remaining 8 vertical samples (4 on each side of the center)
  for (float i = 1.0; i <= numBlurPixelsPerSide; i++) { 
    avgValue += texture2D(blurSampler, gl_TexCoord[0].xy - i * blurSize * 
                          blurMultiplyVec).x * incrementalGaussian.x;         
    avgValue += texture2D(blurSampler, gl_TexCoord[0].xy + i * blurSize * 
                          blurMultiplyVec).x * incrementalGaussian.x;         
    coefficientSum += 2.0 * incrementalGaussian.x;
    incrementalGaussian.xy *= incrementalGaussian.yz;
  }

  avgValue = sqrt(avgValue);
	
  gl_FragColor = vec4(avgValue, avgValue*0.8, avgValue*0.8, avgValue);
}
