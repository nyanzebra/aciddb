PHASE_1_PDF=build/phase-1/phase-1.pdf
PHASE_2_PDF=build/phase-2/phase-2.pdf

.PHONY: $(PHASE_1_PDF) $(PHASE_2_PDF)

all: phase-1-images phase-1 phase-2-images phase-2

phase-1: phase-1-images phase-1-latex

phase-2: phase-2-images phase-2-pseudocode phase-2-latex

phase-1-latex: $(PHASE_1_PDF)

phase-2-latex: $(PHASE_2_PDF)

$(PHASE_1_PDF): phase-1.tex
	@mkdir -p build/phase-1
	latex -output-format=pdf -interaction=batchmode -output-directory=build/phase-1 phase-1.tex

$(PHASE_2_PDF): phase-2.tex
	@mkdir -p build/phase-2
	latex -output-format=pdf -interaction=batchmode -output-directory=build/phase-2 phase-2.tex

phase-1-images:
	plantuml -o ../../build/phase-1/images diagrams/phase-1/*

phase-2-images:
	plantuml -o ../../build/phase-2/images diagrams/phase-2/*

phase-2-pseudocode:
	cp -r code/phase-2/ build/phase-2/code

clean:
	rm -rf build