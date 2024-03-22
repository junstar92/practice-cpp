/** CRTP for generic interface */
#include <iostream>
#include <vector>

struct Animal
{
    enum Type { CAT, DOG, RAT };
    Animal(Type t, char const* n) : type{t}, name{n} {}
    Type const type;
    char const* const name;
};

template<typename D>
class GenericVisitor
{
public:
    template<typename it>
    void visit(it from, it to) {
        for (it i = from; i != to; ++i) {
            this->visit(*i);
        }
    }

private:
    D& derived() {
        return *static_cast<D*>(this);
    }
    void visit(Animal const& animal) {
        switch (animal.type)
        {
        case Animal::CAT:
            derived().visit_cat(animal);
            break;
        case Animal::DOG:
            derived().visit_dog(animal);
            break;
        case Animal::RAT:
            derived().visit_rat(animal);
            break;
        }
    }
    void visit_cat(Animal const& animal) {
        std::cout << "Feed the cat " << animal.name << std::endl;
    }
    void visit_dog(Animal const& animal) {
        std::cout << "Wash the dog " << animal.name << std::endl;
    }
    void visit_rat(Animal const& animal) {
        std::cout << "Eeek!" << std::endl;
    }
    friend D;
    GenericVisitor() {}
};

class DefaultVisitor : public GenericVisitor<DefaultVisitor> {
};

class TrainerVisitor : public GenericVisitor<TrainerVisitor> {
    friend class GenericVisitor<TrainerVisitor>;
    void visit_dog(Animal const& animal) {
        std::cout << "Train the dog " << animal.name << std::endl;
    }
};

class FelineVisitor : public GenericVisitor<FelineVisitor> {
    friend class GenericVisitor<FelineVisitor>;
    void visit_cat(Animal const& animal) {
        std::cout << "Hiss at the cat " << animal.name << std::endl;
    }
    void visit_dog(Animal const& animal) {
        std::cout << "Growl at the dog " << animal.name << std::endl;
    }
    void visit_rat(Animal const& animal) {
        std::cout << "Eat the rat " << animal.name << std::endl;
    }
};

int main() {
    std::vector<Animal> animals {{Animal::CAT, "Fluffy"}, {Animal::DOG, "Fido"}, {Animal::RAT, "Stinky"}};

    std::cout << "Visiting somebody:" << std::endl;
    DefaultVisitor().visit(animals.begin(), animals.end());

    std::cout << "Visiting trainer:" << std::endl;
    TrainerVisitor().visit(animals.begin(), animals.end());

    std::cout << "Visiting cat:" << std::endl;
    FelineVisitor().visit(animals.begin(), animals.end());
}