//
// Created by PrinceXuan on 2017/5/30.
//

#include "Env.h"

namespace Env
{

    Entry::Entry()
    {
        name = "NotDefined";
    }

    Entry::Entry(const std::string &name)
            : name(name)
    {}

    void Entry::setName(const std::string &name)
    {
        Entry::name = name;
    }

    Entry::~Entry()
    {

    }

    TypeEntry::TypeEntry(const std::string &name, const std::shared_ptr<Type::Type> type)
            : Entry(name), type(type)
    {

    }

    const std::shared_ptr<Type::Type> TypeEntry::getType() const
    {
        return type;
    }

    VarEntry::VarEntry()
            : type(std::make_shared<Type::Type>()),
              access(nullptr)
    {}

    VarEntry::VarEntry(const std::string &name, const std::shared_ptr<Type::Type> type,
                       const std::shared_ptr<Translate::Access> access)
            : Entry(name), type(type), access(access)
    {}

    std::shared_ptr<Type::Type> VarEntry::getType() const
    {
        return type;
    }

    void VarEntry::dumpInfo()
    {
        std::cerr << "Var Entry Dump Info" << std::endl;
        std::cerr << "Var name : " << name << std::endl;
        std::cerr << "Var type : " << Type::getName(type) << std::endl;
        std::cerr << std::endl;
    }

    FuncEntry::FuncEntry()
            : args(std::make_shared<ArgList>()),
              result(std::make_shared<Type::Type>()),
              level(nullptr),
              label(nullptr)
    {}

    FuncEntry::FuncEntry(const std::shared_ptr<Translate::Level> level,
                         const std::shared_ptr<Temporary::Label> label,
                         const std::string &name,
                         const std::shared_ptr<Type::Type> argType,
                         const std::shared_ptr<Type::Type> resultType)
            : Entry(name), level(level), label(label), result(resultType)
    {
        ArgList arg;
        arg.push_back(argType);
        args = std::make_shared<ArgList>(arg);
    }

    FuncEntry::FuncEntry(const std::shared_ptr<Translate::Level> level,
                         const std::shared_ptr<Temporary::Label> label,
                         const std::string &name,
                         const std::shared_ptr<Type::Type> result)
            : Entry(name), level(level), label(label), result(result), args(std::make_shared<ArgList>())
    {}

    void FuncEntry::addArg(std::shared_ptr<Type::Type> arg)
    {
        args->push_back(arg);
    }

    const std::shared_ptr<ArgList> FuncEntry::getArgs() const
    {
        return args;
    }

    const std::shared_ptr<Type::Type> FuncEntry::getResultType() const
    {
        return result;
    }

    FuncEntry::FuncEntry(const std::shared_ptr<Translate::Level> level,
                         const std::shared_ptr<Temporary::Label> label,
                         const std::string &name,
                         const std::initializer_list<std::shared_ptr<Type::Type>> &argTypes,
                         const std::shared_ptr<Type::Type> resultType)
            : Entry(name), level(level), label(label), result(resultType)
    {
        args = std::make_shared<ArgList>(argTypes);
    }

    const std::shared_ptr<Translate::Level> FuncEntry::getLevel() const
    {
        return level;
    }

    const std::shared_ptr<Temporary::Label> FuncEntry::getLabel() const
    {
        return label;
    }

    FuncEntry::FuncEntry(const std::shared_ptr<Translate::Level> level, const std::shared_ptr<Temporary::Label> label,
                         const std::string &name, const std::shared_ptr<ArgList> argTypeList,
                         const std::shared_ptr<Type::Type> resultType)
            : Entry(name), args(argTypeList), result(resultType),
              level(level), label(label)
    {}

    Env::Env()
    {}

    Env::~Env()
    {}

    EntryNotFound::EntryNotFound(const std::string &msg)
            : std::runtime_error(msg)
    {}

    TypeEnv::TypeEnv()
    {}

    void TypeEnv::setDefaultEnv()
    {
        TypeEntry intVar("int", Type::INT);
        enter(std::make_shared<TypeEntry>(intVar));
        TypeEntry stringVar("string", Type::STRING);
        enter(std::make_shared<TypeEntry>(stringVar));
        TypeEntry voidVar("void", Type::VOID);
        enter(std::make_shared<TypeEntry>(voidVar));
    }

    void TypeEnv::enter(std::shared_ptr<TypeEntry> entry)
    {
        bindList.push_back(entry);
    }

    std::shared_ptr<TypeEntry> TypeEnv::find(const std::string &name)
    {
        for (auto r_iter = bindList.crbegin(); r_iter != bindList.crend(); r_iter++)
        {
            if ((*r_iter)->name == name)
            {
                return (*r_iter);
            }
        }
        throw EntryNotFound("Var Entry with name " + name + " not found");
    }

    void TypeEnv::beginScope()
    {
        scope.push_back(bindList.size());
    }

    void TypeEnv::endScope()
    {
        auto s = scope.back();
        scope.pop_back();
        bindList.resize(s);
    }

    void TypeEnv::enterType(TypeEntry typeEntry)
    {
        auto typePtr = std::make_shared<TypeEntry>(typeEntry);
        enter(typePtr);
    }

    VarEnv::VarEnv()
    {}

    void VarEnv::setDefaultEnv()
    {
        // print
        FuncEntry print(Translate::getGlobalLevel(),
                        Temporary::makeLabel(),
                        "print", Type::STRING, Type::VOID);
        enterFunc(print);
        // flush
        FuncEntry flush(Translate::getGlobalLevel(),
                        Temporary::makeLabel(),
                        "flush", Type::VOID);
        enterFunc(flush);
        // getchar
        FuncEntry getchar(Translate::getGlobalLevel(),
                          Temporary::makeLabel(),
                          "getchar", Type::STRING);
        enterFunc(getchar);
        // ord
        FuncEntry ord(Translate::getGlobalLevel(),
                      Temporary::makeLabel(),
                      "ord", Type::STRING, Type::INT);
        enterFunc(ord);
        // chr
        FuncEntry chr(Translate::getGlobalLevel(),
                      Temporary::makeLabel(),
                      "chr", Type::INT, Type::STRING);
        enterFunc(chr);
        // size
        FuncEntry size(Translate::getGlobalLevel(),
                       Temporary::makeLabel(),
                       "size", Type::STRING, Type::INT);
        enterFunc(size);
        // substring
        FuncEntry substring(Translate::getGlobalLevel(),
                            Temporary::makeLabel(),
                            "substring", {Type::STRING, Type::INT, Type::INT}, Type::STRING);
        enterFunc(substring);
        // concat
        FuncEntry concat(Translate::getGlobalLevel(),
                         Temporary::makeLabel(),
                         "concat", {Type::STRING, Type::STRING}, Type::STRING);
        enterFunc(concat);
        // not
        FuncEntry notFunc(Translate::getGlobalLevel(),
                          Temporary::makeLabel(),
                          "not", Type::INT, Type::INT);
        enterFunc(notFunc);
        // exit
        FuncEntry exit(Translate::getGlobalLevel(),
                       Temporary::makeLabel(),
                       "exit", Type::INT, Type::VOID);
        enterFunc(exit);
    }

    void VarEnv::enter(std::shared_ptr<Entry> entry)
    {
        this->bindList.push_back(entry);
    }

    std::shared_ptr<Entry> VarEnv::find(const std::string &entryName)
    {
        for (auto r_iter = bindList.crbegin(); r_iter != bindList.crend(); r_iter++)
        {
            if ((*r_iter)->name == entryName)
            {
                return (*r_iter);
            }
        }
        throw EntryNotFound("Entry with name " + entryName + " not found");
    }

    void VarEnv::beginScope()
    {
        scope.push_back(bindList.size());
    }

    void VarEnv::endScope()
    {
        auto s = scope.back();
        scope.pop_back();
        bindList.resize(s);
    }

    std::shared_ptr<VarEntry> VarEnv::findVar(const std::string &varName)
    {
        auto entry = find(varName);
        return std::dynamic_pointer_cast<VarEntry>(entry);
    }

    std::shared_ptr<FuncEntry> VarEnv::findFunc(const std::string &funcName)
    {
        auto entry = find(funcName);
        return std::dynamic_pointer_cast<FuncEntry>(entry);
    }

    void VarEnv::enterFunc(FuncEntry funcEntry)
    {
        auto funcPtr = std::make_shared<FuncEntry>(funcEntry);
        enter(funcPtr);
    }

    void VarEnv::enterVar(VarEntry varEntry)
    {
        auto varPtr = std::make_shared<VarEntry>(varEntry);
        enter(varPtr);
    }

    std::shared_ptr<ArgList> makeArgList()
    {
        return std::make_shared<ArgList>();
    }
}