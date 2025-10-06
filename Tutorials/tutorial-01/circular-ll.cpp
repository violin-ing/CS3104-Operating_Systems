template<class T>
class list_node {
public: 
    explicit list_node(T data) 
        : prev_(this)
        , next_(this)
        , data_(data) {}

    void insert(list_node* n) {
        n->prev_ = this;
        n->next_ = this->next_;
        this->next_->prev_ = n;
        this->next_ = n;
    }

    void remove() {
        this->next_->prev_ = this->prev_;
        this->prev_->next_ = this->next_;
        this->prev_ = this;
        this->next_ = this;
    }

    const T& data() const { return data_; } 

private: 
    list_node *prev_, *next_; 
    T data_; 
};
