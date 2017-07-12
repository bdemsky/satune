#ifndef SATELEMENTENCODER_H
#define SATELEMENTENCODER_H

Edge getElementValueOneHotConstraint(SATEncoder * This, Element* elem, uint64_t value);
Edge getElementValueBinaryIndexConstraint(SATEncoder * This, Element* element, uint64_t value);
Edge getElementValueConstraint(SATEncoder* encoder, Element* This, uint64_t value);
void allocElementConstraintVariables(ElementEncoding* This, uint numVars);
void generateOneHotEncodingVars(SATEncoder *This, ElementEncoding *encoding);
void generateBinaryIndexEncodingVars(SATEncoder* This, ElementEncoding* encoding);
void generateElementEncoding(SATEncoder* This, Element* element);
#endif
