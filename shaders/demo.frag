#version 430

varying vec2 vTexCoords;

void main() {
    gl_FragColor = vec4(vTexCoords, 0., 1.);
}