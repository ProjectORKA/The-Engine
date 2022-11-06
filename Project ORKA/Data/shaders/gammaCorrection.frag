
//! #include "uniforms.glsl"

layout(location = 0) out vec4 fragmentColor;
in vec2 textureCoordinate;

void main(){
    vec4 color = vec4(0);
    float d = distance(vec2(0.5,0.5), textureCoordinate);
    float v = d * d;
    float r = 0.002 * v;
  color += vec4(texture(texture0,textureCoordinate + vec2(r * 0.0, 0.0*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * -6.7549043, -7.3736877*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * 11.846537, 1.0396682*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * -10.444376, 8.007503*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * 2.4632936, -13.925954*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * 8.025981, 12.617069*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * -15.114254, -4.0630374*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * 14.434997, -7.4970675*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * -5.7691736, 15.797448*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * -6.6087337, -16.010141*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * 16.106516, 7.5371037*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * -17.37681, 5.450593*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * 9.33227, -16.103382*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * 4.0772204, 18.545387*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * -15.824174, -11.124804*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * 19.516718, -2.5290427*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * -12.88889, 15.293021*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * -0.8391422, -20.288084*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * 14.529153, 14.600265*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * -20.855692, -0.9643243*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * 16.236542, -13.549476*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * -2.8542812, 21.21581*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * -12.369528, -17.77741*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * 21.36537, 4.806674*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * -19.204214, 11.004368*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * 6.796147, -21.302872*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * 9.47113, 20.498772*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * -21.027946, -8.8000345*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * 21.645563, -7.785877*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * -10.7963, 20.541576*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * -5.9652357, -22.630478*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * 19.847343, 12.761639*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * -23.44047, 4.0286174*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * 14.675351, -18.949678*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * 1.9941226, 24.064882*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * -17.855007, -16.516863*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * 24.494606, 0.11959641*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * -18.26696, 16.57089*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * 2.291115, -24.722301*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * 15.108169, 19.905853*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * -24.74264, -4.5008116*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * 21.416853, -13.477052*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * -6.7280474, 24.552137*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * -11.691674, -22.782637*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * 24.14924, 8.952046*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * -23.98856, 9.765719*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * 11.153485, -23.533653*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * 7.7147703, 25.020945*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * -22.708912, -13.308857*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * 25.867651, -5.5555897*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * -15.399386, 21.678692*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * -3.30598, -26.518171*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * 20.449081, 17.405325*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * -26.963766, 0.98299444*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * 19.307583, -19.027985*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * -1.3906467, 27.197166*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * -17.425068, -21.08788*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * 27.213411, 3.7965364*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * -22.729858, 15.6502695*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * 6.214232, -27.00922*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * 13.7191925, 24.215294*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * -26.583231, -8.623044*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * 25.530298, -11.644942*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * -11.002206, 25.936014*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * -9.443269, -26.661295*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * 25.069262, 13.332589*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * -27.596207, 7.131149*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * 15.590675, -23.988876*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * 4.7266498, 28.324545*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * -22.700487, -17.758102*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * 28.837528, -2.24878*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * -19.815552, 21.212236*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * 0.2844464, -29.128117*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * 19.53403, 21.744469*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * -29.191015, -2.8490646*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * 23.527256, -17.67749*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * -5.42597, 29.023073*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * -15.654296, -25.148348*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * 28.622921, 7.994018*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * -26.590427, 13.48216*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * 10.531964, -27.99116*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * 11.175758, 27.84066*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * -27.129515, -13.02029*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * 28.886635, -8.752471*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * -15.433113, 26.044851*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * -6.228982, -29.71791*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * 24.74008, 17.756414*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * -30.324242, 3.6302075*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * 19.964119, -23.227509*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * 0.96911997, 30.699493*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * -21.512598, -22.042946*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * 30.837648, 1.7259018*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * -23.971653, 19.60934*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * 4.4409304, -30.735048*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * 17.534355, 25.73096*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * -30.390848, -7.1467295*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * 27.309532, -15.29658*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * -9.828882, 29.804007*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * -12.918903, -28.688871*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * 28.977432, 12.46178*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * -29.859426, 10.4122305*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * 15.018677, -27.918219*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * 7.802589, 30.806982*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * -26.627623, -17.48882*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * 31.524092, -5.1024976*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * -19.843195, 25.118574*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * -2.336416, -32.00163*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * 23.39657, 22.068268*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * -32.23367, -0.47021076*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * 24.141836, -21.475622*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * -3.303219, 32.21642*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * -19.372477, -26.043667*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * 31.948303, 6.1324196*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * -27.762135, 17.096155*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * 8.943125, -31.428143*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * 14.66984, 29.277575*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * -30.658363, -11.70887*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * 30.58014, -12.104565*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * -14.403696, 29.644756*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * -9.426824, -31.65448*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * 28.39017, 17.01304*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * -32.49287, 6.6493077*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * 19.508476, -26.906422*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * 3.7969809, 33.085293*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * -25.198969, -21.876122*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * 33.42501, -0.8959049*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * -24.08992, 23.284607*r)));
  color += vec4(texture(texture0,textureCoordinate + vec2(r * 2.0395143, -33.507957*r)));


    color /= 128;
    color *= vec4(vec3(pow(1 - v,1)),1);

    color = vec4(pow(color.rgb, vec3(1.0/2.2)),color.a);
    fragmentColor = color;
}