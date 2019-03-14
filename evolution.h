// function that kills a cockroach, so the array of swarm is reduced by one
Cockroach* adios(Cockroach *swarm, int *swarmSize, int idx_death);

// function that passes a larva cockroach to an adult if it's its day
void cockroach_evolution(Cockroach *insect,int num_insect, int day);
