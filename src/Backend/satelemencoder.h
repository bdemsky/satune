#ifndef SATELEMENTENCODER_H
#define SATELEMENTENCODER_H

Edge getElementValueBinaryIndexConstraint(SATEncoder * This, Element* element, uint64_t value);
Edge getElementValueConstraint(SATEncoder* encoder, Element* This, uint64_t value);

#endif
