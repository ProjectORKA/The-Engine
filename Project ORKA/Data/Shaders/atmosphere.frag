
//! #include "uniforms.glsl"

layout(location = 0) out vec4 gBufferColor;
layout (location = 1) out vec3 gBufferNormal;
layout (location = 2) out vec3 gBufferPosition;
layout (location = 3) out uint gBufferMaterialID;

in vec3 position;

#define SIZE_FACTOR 1.0

#define PLANET_RADIUS 6371000 * SIZE_FACTOR
#define ATMOS_RADIUS 6471000 * SIZE_FACTOR
#define PLANET_POS vec3(0.0,-PLANET_RADIUS, 0)

// scattering coeffs
#define RAY_BETA vec3(5.5e-6, 13.0e-6, 22.4e-6)/* rayleigh, affects the color of the sky */
#define MIE_BETA vec3(21e-6) /* mie, affects the color of the blob around the sun */
#define AMBIENT_BETA vec3(0.0) /* ambient, affects the scattering color when there is no lighting from the sun */
#define ABSORPTION_BETA vec3(2.04e-5, 4.97e-5, 1.95e-6) /* what color gets absorbed by the atmosphere (Due to things like ozone) */
#define G 0.7 /* mie scattering direction, or how big the blob around the sun is */
// and the heights (how far to go up before the scattering has no effect)
#define HEIGHT_RAY 8e3 /* rayleigh height */
#define HEIGHT_MIE 1.2e3 /* and mie */
#define HEIGHT_ABSORPTION 30e3 /* at what height the absorption is at it's maximum */
#define ABSORPTION_FALLOFF 4e3 /* how much the absorption decreases the further away it gets from the maximum height */
// and the steps (more looks better, but is slower)
// the primary step has the most effect on looks
#define PRIMARY_STEPS 16 /* primary steps, affects quality the most */
#define LIGHT_STEPS 4 /* light steps, how much steps in the light direction are taken */

vec3 calculate_scattering(
	vec3 start, 				// the start of the ray (the camera position)
    vec3 dir, 					// the direction of the ray (the camera vector)
    float max_dist, 			// the maximum distance the ray can travel (because something is in the way, like an object)
    vec3 scene_color,			// the color of the scene
    vec3 light_dir, 			// the direction of the light
    vec3 light_intensity,		// how bright the light is, affects the brightness of the atmosphere
    vec3 planet_position, 		// the position of the planet
    float planet_radius, 		// the radius of the planet
    float atmo_radius, 			// the radius of the atmosphere
    vec3 beta_ray, 				// the amount rayleigh scattering scatters the colors (for earth: causes the blue atmosphere)
    vec3 beta_mie, 				// the amount mie scattering scatters colors
    vec3 beta_absorption,   	// how much air is absorbed
    vec3 beta_ambient,			// the amount of scattering that always occurs, cna help make the back side of the atmosphere a bit brighter
    float g, 					// the direction mie scatters the light in (like a cone). closer to -1 means more towards a single direction
    float height_ray, 			// how high do you have to go before there is no rayleigh scattering?
    float height_mie, 			// the same, but for mie
    float height_absorption,	// the height at which the most absorption happens
    float absorption_falloff,	// how fast the absorption falls off from the absorption height
    int steps_i, 				// the amount of steps along the 'primary' ray, more looks better but slower
    int steps_l 				// the amount of steps along the light ray, more looks better but slower
) {
    start -= planet_position;
    float a = dot(dir, dir);
    float b = 2.0 * dot(dir, start);
    float c = dot(start, start) - (atmo_radius * atmo_radius);
    float d = (b * b) - 4.0 * a * c;
    
    if (d < 0.0) return scene_color;

    vec2 ray_length = vec2(
        max((-b - sqrt(d)) / (2.0 * a), 0.0),
        min((-b + sqrt(d)) / (2.0 * a), max_dist)
    );
    if (ray_length.x > ray_length.y) return scene_color;
    bool allow_mie = max_dist > ray_length.y;
    ray_length.y = min(ray_length.y, max_dist);
    ray_length.x = max(ray_length.x, 0.0);
    float step_size_i = (ray_length.y - ray_length.x) / float(steps_i);
    float ray_pos_i = ray_length.x + step_size_i * 0.5;
    vec3 total_ray = vec3(0.0);
    vec3 total_mie = vec3(0.0);
    vec3 opt_i = vec3(0.0);
    vec3 prev_density = vec3(0.0);
    vec2 scale_height = vec2(height_ray, height_mie);
    float mu = dot(dir, light_dir);
    float mumu = mu * mu;
    float gg = g * g;
    float phase_ray = 3.0 / (50.2654824574) * (1.0 + mumu);
    float phase_mie = allow_mie ? 3.0 / (25.1327412287) * ((1.0 - gg) * (mumu + 1.0)) / (pow(1.0 + gg - 2.0 * mu * g, 1.5) * (2.0 + gg)) : 0.0;
    for (int i = 0; i < steps_i; ++i) {
        vec3 pos_i = start + dir * ray_pos_i;
        float height_i = length(pos_i) - planet_radius;
        vec3 density = vec3(exp(-height_i / scale_height), 0.0);
        
        // and the absorption density. this is for ozone, which scales together with the rayleigh, 
        // but absorbs the most at a specific height, so use the sech function for a nice curve falloff for this height
        // clamp it to avoid it going out of bounds. This prevents weird black spheres on the night side
        float denom = max(0,(height_absorption - height_i) / absorption_falloff);
        density.z = max(0,(1.0 / (denom * denom + 1.0)) * density.x);
        
        // multiply it by the step size here
        // we are going to use the density later on as well
        density *= step_size_i;
        
        // Add these densities to the optical depth, so that we know how many particles are on this ray.
        // max here is needed to prevent opt_i from potentially becoming negative
        opt_i += max(density + (prev_density - density) * 0.5, 0.0);
        
        // and update the previous density
        prev_density = density;

        // Calculate the step size of the light ray.
        // again with a ray sphere intersect
        // a, b, c and d are already defined
        a = dot(light_dir, light_dir);
        b = 2.0 * dot(light_dir, pos_i);
        c = dot(pos_i, pos_i) - (atmo_radius * atmo_radius);
        d = (b * b) - 4.0 * a * c;

        // no early stopping, this one should always be inside the atmosphere
        // calculate the ray length
        float step_size_l = (-b + sqrt(d)) / (2.0 * a * float(steps_l));

        // and the position along this ray
        // this time we are sure the ray is in the atmosphere, so set it to 0
        float ray_pos_l = step_size_l * 0.5;

        // and the optical depth of this ray
        vec3 opt_l = vec3(0.0);
        
        // again, use the prev density for better integration
        vec3 prev_density_l = vec3(0.0);
        
        // now sample the light ray
        // this is similar to what we did before
        for (int l = 0; l < steps_l; ++l) {

            // calculate where we are along this ray
            vec3 pos_l = pos_i + light_dir * ray_pos_l;

            // the heigth of the position
            float height_l = length(pos_l) - planet_radius;

            // calculate the particle density, and add it
            // this is a bit verbose
            // first, set the density for ray and mie
            vec3 density_l = vec3(exp(-height_l / scale_height), 0.0);
            
            // then, the absorption
            float denom = (height_absorption - height_l) / absorption_falloff;
            density_l.z = (1.0 / (denom * denom + 1.0)) * density_l.x;
            
            // multiply the density by the step size
            density_l *= step_size_l;
            
            // and add it to the total optical depth
            opt_l += max(density_l + (prev_density_l - density_l) * 0.5, 0.0);
            
            // and update the previous density
            prev_density_l = density_l;

            // and increment where we are along the light ray.
            ray_pos_l += step_size_l;
            
        }
        
        // Now we need to calculate the attenuation
        // this is essentially how much light reaches the current sample point due to scattering
        vec3 attn = exp(-beta_ray * (opt_i.x + opt_l.x) - beta_mie * (opt_i.y + opt_l.y) - beta_absorption * (opt_i.z + opt_l.z));

        // accumulate the scattered light (how much will be scattered towards the camera)
        total_ray += density.x * attn;
        total_mie += density.y * attn;

        // and increment the position on this ray
        ray_pos_i += step_size_i;
    	
    }
    
    // calculate how much light can pass through the atmosphere
    vec3 opacity = exp(-(beta_mie * opt_i.y + beta_ray * opt_i.x + beta_absorption * opt_i.z));
    
	// calculate and return the final color
    return (
        	phase_ray * beta_ray * total_ray // rayleigh color
       		+ phase_mie * beta_mie * total_mie // mie
            + opt_i.x * beta_ambient // and ambient
    ) * light_intensity + scene_color * opacity; // now make sure the background is rendered correctly
}

/*
A ray-sphere intersect
This was previously used in the atmosphere as well, but it's only used for the planet intersect now, since the atmosphere has this
ray sphere intersect built in
*/

vec2 ray_sphere_intersect(
    vec3 start, // starting position of the ray
    vec3 dir, // the direction of the ray
    float radius // and the sphere radius
) {
    // ray-sphere intersection that assumes
    // the sphere is centered at the origin.
    // No intersection when result.x > result.y
    float a = dot(dir, dir);
    float b = 2.0 * dot(dir, start);
    float c = dot(start, start) - (radius * radius);
    float d = (b*b) - 4.0*a*c;
    if (d < 0.0) return vec2(1e5,-1e5);
    return vec2(
        (-b - sqrt(d))/(2.0*a),
        (-b + sqrt(d))/(2.0*a)
    );
}

/*
To make the planet we're rendering look nicer, we implemented a skylight function here

Essentially it just takes a sample of the atmosphere in the direction of the surface normal
*/
vec3 skylight(vec3 sample_pos, vec3 surface_normal, vec3 light_dir, vec3 background_col) {

    // slightly bend the surface normal towards the light direction
    surface_normal = normalize(mix(surface_normal, light_dir, 0.6));
    
    // and sample the atmosphere
    return calculate_scattering(
    	sample_pos,						// the position of the camera
        surface_normal, 				// the camera vector (ray direction of this pixel)
        3.0 * ATMOS_RADIUS, 			// max dist, since nothing will stop the ray here, just use some arbitrary value
        background_col,					// scene color, just the background color here
        light_dir,						// light direction
        vec3(40.0),						// light intensity, 40 looks nice
        PLANET_POS,						// position of the planet
        PLANET_RADIUS,                  // radius of the planet in meters
        ATMOS_RADIUS,                   // radius of the atmosphere in meters
        RAY_BETA,						// Rayleigh scattering coefficient
        MIE_BETA,                       // Mie scattering coefficient
        ABSORPTION_BETA,                // Absorbtion coefficient
        AMBIENT_BETA,					// ambient scattering, turned off for now. This causes the air to glow a bit when no light reaches it
        G,                          	// Mie preferred scattering direction
        HEIGHT_RAY,                     // Rayleigh scale height
        HEIGHT_MIE,                     // Mie scale height
        HEIGHT_ABSORPTION,				// the height at which the most absorption happens
        ABSORPTION_FALLOFF,				// how fast the absorption falls off from the absorption height
        LIGHT_STEPS, 					// steps in the ray direction
        LIGHT_STEPS 					// steps in the light direction
    );
}

/*
The following function returns the scene color and depth 
(the color of the pixel without the atmosphere, and the distance to the surface that is visible on that pixel)

in this case, the function renders a green sphere on the place where the planet should be
color is in .xyz, distance in .w

I won't explain too much about how this works, since that's not the aim of this shader
*/
vec4 render_scene(vec3 pos, vec3 dir, vec3 light_dir) {
    
    // the color to use, w is the scene depth
    vec4 color = vec4(0.0, 0.0, 0.0, 1e12);
    
    // add a sun, if the angle between the ray direction and the light direction is small enough, color the pixels white
    color.xyz = vec3(dot(dir, light_dir) > 0.9998 ? 3.0 : 0.0);
    
    // get where the ray intersects the planet
    vec2 planet_intersect = ray_sphere_intersect(pos - PLANET_POS, dir, PLANET_RADIUS-10); 
    
    // if the ray hit the planet, set the max distance to that ray
    if (0.0 < planet_intersect.y) {
    	color.w = max(planet_intersect.x, 0.0);
        
        // sample position, where the pixel is
        vec3 sample_pos = pos + (dir * planet_intersect.x) - PLANET_POS;
        
        // and the surface normal
        vec3 surface_normal = normalize(sample_pos);
        
        // get the color of the sphere
        color.xyz = vec3(0.0, 0.5, 0.7); 
        
        // get wether this point is shadowed, + how much light scatters towards the camera according to the lommel-seelinger law
        vec3 N = surface_normal;
        vec3 V = -dir;
        vec3 L = light_dir;
        float dotNV = max(1e-6, dot(N, V));
        float dotNL = max(1e-6, dot(N, L));
        float shadow = dotNL / (dotNL + dotNV);
        
        // apply the shadow
        color.xyz *= shadow;
        
        // apply skylight
        color.xyz += clamp(skylight(sample_pos, surface_normal, light_dir, vec3(0.0)) * vec3(0.0, 0.25, 0.05), 0.0, 1.0);
    }
    
	return color;
}

void main() {
    vec3 cameraVector = normalize(position.xzy);
    vec3 camera_position = vec3(ATMOS_RADIUS * 2 * cameraPosition.xzy);
    vec3 light_dir = sunDir.xzy;
    vec4 scene = render_scene(camera_position, cameraVector, light_dir);
    scene.xyz = vec3(0);//texture(texture0,gl_FragCoord.xy/vec2(width,height)).xyz;
    //scene.w = 
    vec3 col = calculate_scattering(
    	camera_position,				// the position of the camera
        cameraVector, 					// the camera vector (ray direction of this pixel)
        scene.w, 						// max dist, essentially the scene depth
        scene.xyz,                        // scene color, the color of the current pixel being rendered
        light_dir,						// light direction
        vec3(40.0),						// light intensity, 40 looks nice
        PLANET_POS,					    // position of the planet
        PLANET_RADIUS,                  // radius of the planet in meters
        ATMOS_RADIUS,                   // radius of the atmosphere in meters
        RAY_BETA,						// Rayleigh scattering coefficient
        MIE_BETA,                       // Mie scattering coefficient
        ABSORPTION_BETA,                // Absorbtion coefficient
        AMBIENT_BETA,					// ambient scattering, turned off for now. This causes the air to glow a bit when no light reaches it
        G,                          	// Mie preferred scattering direction
        HEIGHT_RAY,                     // Rayleigh scale height
        HEIGHT_MIE,                     // Mie scale height
        HEIGHT_ABSORPTION,				// the height at which the most absorption happens
        ABSORPTION_FALLOFF,				// how fast the absorption falls off from the absorption height 
        PRIMARY_STEPS, 					// steps in the ray direction 
        LIGHT_STEPS 					// steps in the light direction
    );
        
    // apply exposure, removing this makes the brighter colors look ugly
    // you can play around with removing this
    col = 1.0 - exp(-col);

    //col = texture(texture1,gl_FragCoord.xy / vec2(width,height)).xyz;

    gBufferColor = vec4(col, 1);
    gBufferPosition = position;
    gBufferNormal = position;
    gBufferMaterialID = 0;
}