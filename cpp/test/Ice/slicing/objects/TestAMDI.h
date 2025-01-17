// **********************************************************************
//
// Copyright (c) 2003-2015 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef TESTI_H
#define TESTI_H

#include <ServerPrivateAMD.h>

class TestI : public virtual ::Test::TestIntf
{
public:

    TestI();
#ifdef ICE_CPP11_MAPPING
    virtual void SBaseAsObject_async(std::function<void (const std::shared_ptr<Ice::Value>&)>,
                                     std::function<void (std::exception_ptr)>,
                                     const ::Ice::Current&);

    virtual void SBaseAsSBase_async(std::function<void (const std::shared_ptr<Test::SBase>&)>,
                                    std::function<void (std::exception_ptr)>,
                                    const ::Ice::Current&);

    virtual void SBSKnownDerivedAsSBase_async(std::function<void (const std::shared_ptr<Test::SBase>&)>,
                                              std::function<void (std::exception_ptr)>,
                                              const ::Ice::Current&);

    virtual void SBSKnownDerivedAsSBSKnownDerived_async(std::function<void (const std::shared_ptr<Test::SBSKnownDerived>&)>,
                                                        std::function<void (std::exception_ptr)>,
                                                        const ::Ice::Current&);

    virtual void SBSUnknownDerivedAsSBase_async(std::function<void (const std::shared_ptr<Test::SBase>&)>,
                                                std::function<void (std::exception_ptr)>,
                                                const ::Ice::Current&);

    virtual void SBSUnknownDerivedAsSBaseCompact_async(std::function<void (const std::shared_ptr<Test::SBase>&)>,
                                                       std::function<void (std::exception_ptr)>,
                                                       const ::Ice::Current&);

    virtual void SUnknownAsObject_async(std::function<void (const std::shared_ptr<Ice::Value>&)>,
                                        std::function<void (std::exception_ptr)>,
                                        const ::Ice::Current&);

    virtual void checkSUnknown_async(std::shared_ptr<Ice::Value>,
                                     std::function<void ()>,
                                     std::function<void (std::exception_ptr)>,
                                     const ::Ice::Current&);

    virtual void oneElementCycle_async(std::function<void (const std::shared_ptr<Test::B>&)>,
                                       std::function<void (std::exception_ptr)>,
                                       const ::Ice::Current&);

    virtual void twoElementCycle_async(std::function<void (const std::shared_ptr<Test::B>&)>,
                                       std::function<void (std::exception_ptr)>,
                                       const ::Ice::Current&);

    virtual void D1AsB_async(std::function<void (const std::shared_ptr<Test::B>&)>,
                             std::function<void (std::exception_ptr)>,
                             const ::Ice::Current&);

    virtual void D1AsD1_async(std::function<void (const std::shared_ptr<Test::D1>&)>,
                              std::function<void (std::exception_ptr)>,
                              const ::Ice::Current&);

    virtual void D2AsB_async(std::function<void (const std::shared_ptr<Test::B>&)>,
                             std::function<void (std::exception_ptr)>,
                             const ::Ice::Current&);

    virtual void paramTest1_async(std::function<void (const std::shared_ptr<Test::B>&,
                                                      const std::shared_ptr<Test::B>&)>,
                                  std::function<void (std::exception_ptr)>,
                                  const ::Ice::Current&);

    virtual void paramTest2_async(std::function<void (const std::shared_ptr<Test::B>&, 
                                                      const std::shared_ptr<Test::B>&)>,
                                  std::function<void (std::exception_ptr)>,
                                  const ::Ice::Current&);

    virtual void paramTest3_async(std::function<void (const std::shared_ptr<Test::B>&, 
                                                      const std::shared_ptr<Test::B>&,
                                                      const std::shared_ptr<Test::B>&)>,
                                  std::function<void (std::exception_ptr)>,
                                  const ::Ice::Current&);

    virtual void paramTest4_async(std::function<void (const std::shared_ptr<Test::B>&, 
                                                      const std::shared_ptr<Test::B>&)>,
                                  std::function<void (std::exception_ptr)>,
                                  const ::Ice::Current&);

    virtual void returnTest1_async(std::function<void (const std::shared_ptr<Test::B>&, 
                                                       const std::shared_ptr<Test::B>&,
                                                       const std::shared_ptr<Test::B>&)>,
                                   std::function<void (std::exception_ptr)>,
                                   const ::Ice::Current&);

    virtual void returnTest2_async(std::function<void (const std::shared_ptr<Test::B>&, 
                                                       const std::shared_ptr<Test::B>&,
                                                       const std::shared_ptr<Test::B>&)>,
                                   std::function<void (std::exception_ptr)>,
                                   const ::Ice::Current&);

    virtual void returnTest3_async(std::shared_ptr<::Test::B>,
                                   std::shared_ptr<::Test::B>,
                                   std::function<void (const std::shared_ptr<Test::B>&)>,
                                   std::function<void (std::exception_ptr)>,
                                   const ::Ice::Current&);

    virtual void sequenceTest_async(std::shared_ptr<::Test::SS1>,
                                    std::shared_ptr<::Test::SS2>,
                                    std::function<void (const ::Test::SS3&)>,
                                    std::function<void (std::exception_ptr)>,
                                    const ::Ice::Current&);

    virtual void dictionaryTest_async(Test::BDict,
                                      std::function<void (const ::Test::BDict&, const ::Test::BDict&)>,
                                      std::function<void (std::exception_ptr)>,
                                      const ::Ice::Current&);

    virtual void exchangePBase_async(std::shared_ptr<::Test::PBase>,
                                     std::function<void (const std::shared_ptr<::Test::PBase>&)>,
                                     std::function<void (std::exception_ptr)>,
                                     const ::Ice::Current&);

    virtual void PBSUnknownAsPreserved_async(std::function<void (const std::shared_ptr<::Test::Preserved>&)>,
                                             std::function<void (std::exception_ptr)>,
                                             const ::Ice::Current&);

    virtual void checkPBSUnknown_async(std::shared_ptr<::Test::Preserved>,
                                       std::function<void ()>,
                                       std::function<void (std::exception_ptr)>,
                                       const ::Ice::Current&);

    virtual void PBSUnknownAsPreservedWithGraph_async(std::function<void (const std::shared_ptr<::Test::Preserved>&)>,
                                                      std::function<void (std::exception_ptr)>,
                                                      const ::Ice::Current&);

    virtual void checkPBSUnknownWithGraph_async(std::shared_ptr<::Test::Preserved>,
                                                std::function<void ()>,
                                                std::function<void (std::exception_ptr)>,
                                                const ::Ice::Current&);

    virtual void PBSUnknown2AsPreservedWithGraph_async(std::function<void (const std::shared_ptr<::Test::Preserved>&)>,
                                                       std::function<void (std::exception_ptr)>,
                                                       const ::Ice::Current&);

    virtual void checkPBSUnknown2WithGraph_async(std::shared_ptr<::Test::Preserved>,
                                                 std::function<void ()>,
                                                 std::function<void (std::exception_ptr)>,
                                                 const ::Ice::Current&);

    virtual void exchangePNode_async(std::shared_ptr<::Test::PNode>,
                                     std::function<void (const std::shared_ptr<::Test::PNode>&)>,
                                     std::function<void (std::exception_ptr)>,
                                     const ::Ice::Current&);

    virtual void throwBaseAsBase_async(std::function<void ()>,
                                       std::function<void (std::exception_ptr)>,
                                       const ::Ice::Current&);

    virtual void throwDerivedAsBase_async(std::function<void ()>,
                                          std::function<void (std::exception_ptr)>,
                                          const ::Ice::Current&);

    virtual void throwDerivedAsDerived_async(std::function<void ()>,
                                             std::function<void (std::exception_ptr)>,
                                             const ::Ice::Current&);

    virtual void throwUnknownDerivedAsBase_async(std::function<void ()>,
                                                 std::function<void (std::exception_ptr)>,
                                                 const ::Ice::Current&);

    virtual void throwPreservedException_async(std::function<void ()>,
                                               std::function<void (std::exception_ptr)>,
                                               const ::Ice::Current&);

    virtual void useForward_async(std::function<void (const std::shared_ptr<::Test::Forward>&)>,
                                  std::function<void (std::exception_ptr)>,
                                  const ::Ice::Current&);

    virtual void shutdown_async(std::function<void ()>,
                                std::function<void (std::exception_ptr)>,
                                const ::Ice::Current&);
#else    
    virtual void SBaseAsObject_async(const ::Test::AMD_TestIntf_SBaseAsObjectPtr&, const ::Ice::Current&);
    virtual void SBaseAsSBase_async(const ::Test::AMD_TestIntf_SBaseAsSBasePtr&, const ::Ice::Current&);
    virtual void SBSKnownDerivedAsSBase_async(const ::Test::AMD_TestIntf_SBSKnownDerivedAsSBasePtr&,
                                              const ::Ice::Current&);
    virtual void SBSKnownDerivedAsSBSKnownDerived_async(const ::Test::AMD_TestIntf_SBSKnownDerivedAsSBSKnownDerivedPtr&,
                                                        const ::Ice::Current&);

    virtual void SBSUnknownDerivedAsSBase_async(const ::Test::AMD_TestIntf_SBSUnknownDerivedAsSBasePtr&,
                                                const ::Ice::Current&);

    virtual void SBSUnknownDerivedAsSBaseCompact_async(const ::Test::AMD_TestIntf_SBSUnknownDerivedAsSBaseCompactPtr&,
                                                       const ::Ice::Current&);

    virtual void SUnknownAsObject_async(const ::Test::AMD_TestIntf_SUnknownAsObjectPtr&, const ::Ice::Current&);
    virtual void checkSUnknown_async(const ::Test::AMD_TestIntf_checkSUnknownPtr&, const Ice::ObjectPtr&,
                                     const ::Ice::Current&);

    virtual void oneElementCycle_async(const ::Test::AMD_TestIntf_oneElementCyclePtr&, const ::Ice::Current&);
    virtual void twoElementCycle_async(const ::Test::AMD_TestIntf_twoElementCyclePtr&, const ::Ice::Current&);

    virtual void D1AsB_async(const ::Test::AMD_TestIntf_D1AsBPtr&, const ::Ice::Current&);
    virtual void D1AsD1_async(const ::Test::AMD_TestIntf_D1AsD1Ptr&, const ::Ice::Current&);
    virtual void D2AsB_async(const ::Test::AMD_TestIntf_D2AsBPtr&, const ::Ice::Current&);

    virtual void paramTest1_async(const ::Test::AMD_TestIntf_paramTest1Ptr&, const ::Ice::Current&);
    virtual void paramTest2_async(const ::Test::AMD_TestIntf_paramTest2Ptr&, const ::Ice::Current&);
    virtual void paramTest3_async(const ::Test::AMD_TestIntf_paramTest3Ptr&, const ::Ice::Current&);
    virtual void paramTest4_async(const ::Test::AMD_TestIntf_paramTest4Ptr&, const ::Ice::Current&);

    virtual void returnTest1_async(const ::Test::AMD_TestIntf_returnTest1Ptr&, const ::Ice::Current&);
    virtual void returnTest2_async(const ::Test::AMD_TestIntf_returnTest2Ptr&, const ::Ice::Current&);
    virtual void returnTest3_async(const ::Test::AMD_TestIntf_returnTest3Ptr&, const ::Test::BPtr&, const ::Test::BPtr&,
                                   const ::Ice::Current&);

    virtual void sequenceTest_async(const ::Test::AMD_TestIntf_sequenceTestPtr&,
                                    const ::Test::SS1Ptr&, const ::Test::SS2Ptr&, const ::Ice::Current&);

    virtual void dictionaryTest_async(const ::Test::AMD_TestIntf_dictionaryTestPtr&,
                                      const ::Test::BDict&, const ::Ice::Current&);

    virtual void exchangePBase_async(const ::Test::AMD_TestIntf_exchangePBasePtr&, const ::Test::PBasePtr&,
                                     const ::Ice::Current&);

    virtual void PBSUnknownAsPreserved_async(const ::Test::AMD_TestIntf_PBSUnknownAsPreservedPtr&,
                                             const ::Ice::Current&);
    virtual void checkPBSUnknown_async(const ::Test::AMD_TestIntf_checkPBSUnknownPtr&, const ::Test::PreservedPtr&,
                                       const ::Ice::Current&);

    virtual void PBSUnknownAsPreservedWithGraph_async(const ::Test::AMD_TestIntf_PBSUnknownAsPreservedWithGraphPtr&,
                                                      const ::Ice::Current&);
    virtual void checkPBSUnknownWithGraph_async(const ::Test::AMD_TestIntf_checkPBSUnknownWithGraphPtr&,
                                                const ::Test::PreservedPtr&, const ::Ice::Current&);

    virtual void PBSUnknown2AsPreservedWithGraph_async(const ::Test::AMD_TestIntf_PBSUnknown2AsPreservedWithGraphPtr&,
                                                       const ::Ice::Current&);
    virtual void checkPBSUnknown2WithGraph_async(const ::Test::AMD_TestIntf_checkPBSUnknown2WithGraphPtr&,
                                                 const ::Test::PreservedPtr&, const ::Ice::Current&);

    virtual void exchangePNode_async(const ::Test::AMD_TestIntf_exchangePNodePtr&, const ::Test::PNodePtr&,
                                     const ::Ice::Current&);

    virtual void throwBaseAsBase_async(const ::Test::AMD_TestIntf_throwBaseAsBasePtr&, const ::Ice::Current&);
    virtual void throwDerivedAsBase_async(const ::Test::AMD_TestIntf_throwDerivedAsBasePtr&, const ::Ice::Current&);
    virtual void throwDerivedAsDerived_async(const ::Test::AMD_TestIntf_throwDerivedAsDerivedPtr&,
                                             const ::Ice::Current&);
    virtual void throwUnknownDerivedAsBase_async(const ::Test::AMD_TestIntf_throwUnknownDerivedAsBasePtr&,
                                                 const ::Ice::Current&);
    virtual void throwPreservedException_async(const ::Test::AMD_TestIntf_throwPreservedExceptionPtr&,
                                               const ::Ice::Current&);

    virtual void useForward_async(const ::Test::AMD_TestIntf_useForwardPtr&, const ::Ice::Current&);

    virtual void shutdown_async(const ::Test::AMD_TestIntf_shutdownPtr&, const ::Ice::Current&);
#endif
};

#endif
