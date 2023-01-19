//
// Created by Emmett on 11/10/2022.
//

#include <algorithm>
#include "genomeView.h"
#include "genome/gene/bodyGene.h"
#include "genome/gene/foodGene.h"

#include "phenome.h"

Sense::Sense(int x, int y, int sense) : x(x), y(y), sense(sense) {}

/**
 * INTERNAL USE FOR GENOME DECODER
 * points i to after the pattern
 * @return the base value of double pattern found, or -1 if end reached
 */
auto seekDoubles(const Genome & genome, i32 & i) -> i32 {
    if (i >= genome.size() - 1) return true;

    auto last = genome.get(i);
    for (++i; i < genome.size(); ++i) {
        auto current = genome.get(i);
        if (last == current) {
            ++i;
            return last;
        }

        last = current;
    }

    return -1;
}

/**
 * INTERNAL USE FOR GENOME DECODER
 * points i to after the chunk read
 * @return 0 length if end of genome is reached
 */
auto readNBases(const Genome & genome, i32 & i, i32 length) -> GenomeView {
    if (i + length > genome.size()) return {};

    auto view = GenomeView { &genome, i, length };

	i += length;

	return view;
}

Phenome::Phenome(Genome & genome):
	body(2, 1),
	foodStats(),
	senses(),
	eyeReactions(),
	environmentReactions()
{
	if (genome.size() == 0) return;

	auto bodyBuilder = BodyBuilder();

    auto i = 0;
    while (true) {
		auto geneType = seekDoubles(genome, i);

        if (geneType == Genome::A) {
	        auto gene = readNBases(genome, i, 7);
	        if (gene.empty()) break;

	        auto bodyGene = BodyGene(gene);

	        for (auto d = 0; d <= bodyGene.duplicate; ++d) {
		        body.addPart(bodyBuilder, bodyGene.direction, bodyGene.type, bodyGene.usingAnchor);
	        }

	        if (bodyGene.setsAnchor()) {
		        bodyBuilder.anchors[bodyGene.setAnchor] = { bodyBuilder.currentX, bodyBuilder.currentY };
	        }

			//TODO populate senses once we figure out which body parts are sense cells

		} else if (geneType == Genome::B) {
	        auto gene = readNBases(genome, i, 12);
	        if (gene.empty()) break;

	        auto senseType = Gene::read2(gene, 0);

			if (senseType == 0) {
				eyeReactions.emplace_back(gene);
			} else {
				environmentReactions.emplace_back(gene);
			}

		} else if (geneType == Genome::C) {
			auto gene = readNBases(genome, i, 6);
			if (gene.empty()) break;

			auto foodGene = FoodGene(gene);
			auto & foodStat = foodStats[foodGene.foodType];
			foodStat.digestionBonus += foodGene.digestionBonus();
	        foodStat.absoprtionBonus += foodGene.absorptionBonus();

		} else if (geneType == Genome::D) {
			auto gene = readNBases(genome, i, 8);
			if (gene.empty()) break;

			auto bonuses = MutationRateGene(gene).getResults();
			mutationModifiers[MutationRateGene::SUBSTITUTION] += bonuses.substitution;
	        mutationModifiers[MutationRateGene::INSERTION] += bonuses.insertion;
	        mutationModifiers[MutationRateGene::DELETION] += bonuses.deletion;

        } else {
	        break;
		}
    }

	auto descendingPriority = [](ReactionGene & left, ReactionGene & right) {
		return left.priority > right.priority;
	};

	std::sort(eyeReactions.begin(), eyeReactions.end(), descendingPriority);
	std::sort(environmentReactions.begin(), environmentReactions.end(), descendingPriority);
}
