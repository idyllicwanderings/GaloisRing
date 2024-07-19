
void sacCheck(x, y, z) {

    // TODO: generate sharing of witness x, y, z
    // TODO: langrange + reconstruct 
    std::vector<GRT<>> x_shares = langrange_compute(x);

    //lift x, y, z
    std::vector<GRT1e<k, d2>> lift_x;

    // generate random a and a*y = c
    a = GR<>::random_element();
    c = multiply(a, y);

    // random eplison
    epsilon = GR<>::random_element();

    // reconstruct alpha 
    alpha = reconstruct(multiply(epsilon, a) - lift_x);

    // zero check 
    multiply(epsilon, lift_z ) - c - multiply(alpha, lift_y) == 0;


}