#pragma once

class SerializationExample {
public:
    void imgui()
    {
        ImGui::InputText("A string", &my_string);
    }

private:
    std::string my_string = "This field is serialized! If you modify it, close the app and re-open it, your changes will persist!";

private:
    friend class cereal::access;
    template<class Archive>
    void serialize(Archive& archive)
    {
        archive(
            cereal::make_nvp("A string", my_string));
    }
    // Check the documentation of Cereal for full information on all the things you can do!
    // https://uscilab.github.io/cereal/
    // PS : Cereal is amazing!
};