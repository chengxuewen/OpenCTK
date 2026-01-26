/***********************************************************************************************************************
**
** Library: OpenCTK
**
** Copyright (C) 2026~Present ChengXueWen.
** Copyright (c) 2023 The WebRTC project authors. All Rights Reserved.
**
** License: MIT License
**
** Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
** documentation files (the "Software"), to deal in the Software without restriction, including without limitation
** the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
** and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
**
** The above copyright notice and this permission notice shall be included in all copies or substantial portions
** of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
** TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
** THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
** CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
** IN THE SOFTWARE.
**
***********************************************************************************************************************/

#include <octk_media_context.hpp>
#include <octk_media_context_factory.hpp>
#include <octk_string_view.hpp>
#include <octk_field_trials_view.hpp>
#include <octk_string_view.hpp>
#include <octk_timestamp.hpp>
#include <octk_invocable.hpp>
#include <octk_clock.hpp>
#include <octk_optional.hpp>

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

OCTK_BEGIN_NAMESPACE

namespace
{

using ::testing::ElementsAre;
using ::testing::IsEmpty;
using ::testing::Not;
using ::testing::NotNull;
using ::testing::Ref;

class FakeEvent : public MediaEvent
{
public:
    Type GetType() const override { return MediaEvent::Type::FakeEvent; }
    bool IsConfigEvent() const override { return false; }
};

class FakeFieldTrials : public FieldTrialsView
{
public:
    explicit FakeFieldTrials(Invocable<void() &&> on_destroyed = nullptr)
        : on_destroyed_(std::move(on_destroyed))
    {
    }
    ~FakeFieldTrials() override
    {
        if (on_destroyed_ != nullptr)
        {
            std::move(on_destroyed_)();
        }
    }

    std::string Lookup(StringView /* key */) const override { return "fake"; }

private:
    Invocable<void() &&> on_destroyed_;
};

class FakeTaskQueueFactory : public TaskQueueFactory
{
public:
    explicit FakeTaskQueueFactory(Invocable<void() &&> on_destroyed = nullptr)
        : on_destroyed_(std::move(on_destroyed))
    {
    }
    ~FakeTaskQueueFactory() override
    {
        if (on_destroyed_ != nullptr)
        {
            std::move(on_destroyed_)();
        }
    }

    std::unique_ptr<TaskQueueBase, TaskQueueBase::Deleter> CreateTaskQueue(StringView /* name */,
                                                                           Priority /* priority */) const override
    {
        return nullptr;
    }

private:
    Invocable<void() &&> on_destroyed_;
};

TEST(MediaContextTest, DefaultMediaContextHasAllUtilities)
{
    MediaContext env = MediaContextFactory().Create();

    // Try to use each utility, expect no crashes.
    env.clock().CurrentTime();
    EXPECT_THAT(env.task_queue_factory().CreateTaskQueue("test", TaskQueueFactory::Priority::kNormal), NotNull());
    env.event_log().Log(std::make_unique<FakeEvent>());
    env.field_trials().Lookup("WebRTC-Debugging-RtpDump");
}

TEST(MediaContextTest, UsesProvidedUtilitiesWithOwnership)
{
    auto owned_field_trials = std::make_unique<FakeFieldTrials>();
    auto owned_task_queue_factory = std::make_unique<FakeTaskQueueFactory>();
    auto owned_clock = std::make_unique<SimulatedClock>(Timestamp::Zero());
    auto owned_event_log = std::make_unique<MediaEventLogNull>();

    FieldTrialsView &field_trials = *owned_field_trials;
    TaskQueueFactory &task_queue_factory = *owned_task_queue_factory;
    Clock &clock = *owned_clock;
    MediaEventLog &event_log = *owned_event_log;

    MediaContext env = CreateMediaContext(std::move(owned_field_trials),
                                          std::move(owned_clock),
                                          std::move(owned_task_queue_factory),
                                          std::move(owned_event_log));

    EXPECT_THAT(env.field_trials(), Ref(field_trials));
    EXPECT_THAT(env.task_queue_factory(), Ref(task_queue_factory));
    EXPECT_THAT(env.clock(), Ref(clock));
    EXPECT_THAT(env.event_log(), Ref(event_log));
}

TEST(MediaContextTest, UsesProvidedUtilitiesWithoutOwnership)
{
    FakeFieldTrials field_trials;
    FakeTaskQueueFactory task_queue_factory;
    SimulatedClock clock(Timestamp::Zero());
    MediaEventLogNull event_log;

    MediaContext env = CreateMediaContext(&field_trials, &clock, &task_queue_factory, &event_log);

    EXPECT_THAT(env.field_trials(), Ref(field_trials));
    EXPECT_THAT(env.task_queue_factory(), Ref(task_queue_factory));
    EXPECT_THAT(env.clock(), Ref(clock));
    EXPECT_THAT(env.event_log(), Ref(event_log));
}

TEST(MediaContextTest, UsesLastProvidedUtility)
{
    auto owned_field_trials1 = std::make_unique<FakeFieldTrials>();
    auto owned_field_trials2 = std::make_unique<FakeFieldTrials>();
    FieldTrialsView &field_trials2 = *owned_field_trials2;

    MediaContext env = CreateMediaContext(std::move(owned_field_trials1), std::move(owned_field_trials2));

    EXPECT_THAT(env.field_trials(), Ref(field_trials2));
}

// Utilities can be provided from different sources, and when some source
// choose not to provide an utility, it is usually expressed with nullptr.
// When utility is not provided, it is natural to use previously set one.
// E.g. Both PeerConnectionFactoryDependencies and PeerConnectionDependencies
// provide field trials. When PeerConnectionDependencies::trials == nullptr,
// then trials from the PeerConnectionFactoryDependencies should be used.
// With nullptr accepted and ignored this can be expressed by
// `Environemt env = CreateMediaContext(pcf_deps.trials, pc_deps.trials);`
// That would use pc_deps.trials when not nullptr, pcf_deps.trials when
// pc_deps.trials is nullptr, but pcf_deps.trials is not, and default field
// trials when both are nullptr.
TEST(MediaContextTest, IgnoresProvidedNullptrUtility)
{
    auto owned_field_trials = std::make_unique<FakeFieldTrials>();
    std::unique_ptr<FieldTrialsView> null_field_trials = nullptr;
    FieldTrialsView &field_trials = *owned_field_trials;

    MediaContext env = CreateMediaContext(std::move(owned_field_trials), std::move(null_field_trials));

    EXPECT_THAT(env.field_trials(), Ref(field_trials));
}

TEST(MediaContextTest, KeepsUtilityAliveWhileMediaContextIsAlive)
{
    bool utility_destroyed = false;
    auto field_trials = std::make_unique<FakeFieldTrials>(
        /*on_destroyed=*/[&] { utility_destroyed = true; });

    // Wrap MediaContext into optional to have explicit control when it is deleted.
    Optional<MediaContext> env = CreateMediaContext(std::move(field_trials));

    EXPECT_FALSE(utility_destroyed);
    env = utils::nullopt;
    EXPECT_TRUE(utility_destroyed);
}

TEST(MediaContextTest, KeepsUtilityAliveWhileCopyOfMediaContextIsAlive)
{
    bool utility_destroyed = false;
    auto field_trials = std::make_unique<FakeFieldTrials>(
        /*on_destroyed=*/[&] { utility_destroyed = true; });

    Optional<MediaContext> env1 = CreateMediaContext(std::move(field_trials));
    Optional<MediaContext> env2 = env1;

    EXPECT_FALSE(utility_destroyed);
    env1 = utils::nullopt;
    EXPECT_FALSE(utility_destroyed);
    env2 = utils::nullopt;
    EXPECT_TRUE(utility_destroyed);
}

TEST(MediaContextTest, FactoryCanBeReusedToCreateDifferentMediaContexts)
{
    auto owned_task_queue_factory = std::make_unique<FakeTaskQueueFactory>();
    auto owned_field_trials1 = std::make_unique<FakeFieldTrials>();
    auto owned_field_trials2 = std::make_unique<FakeFieldTrials>();
    TaskQueueFactory &task_queue_factory = *owned_task_queue_factory;
    FieldTrialsView &field_trials1 = *owned_field_trials1;
    FieldTrialsView &field_trials2 = *owned_field_trials2;

    MediaContextFactory factory;
    factory.Set(std::move(owned_task_queue_factory));
    factory.Set(std::move(owned_field_trials1));
    MediaContext env1 = factory.Create();
    factory.Set(std::move(owned_field_trials2));
    MediaContext env2 = factory.Create();

    // MediaContexts share the same custom task queue factory.
    EXPECT_THAT(env1.task_queue_factory(), Ref(task_queue_factory));
    EXPECT_THAT(env2.task_queue_factory(), Ref(task_queue_factory));

    // MediaContexts have different field trials.
    EXPECT_THAT(env1.field_trials(), Ref(field_trials1));
    EXPECT_THAT(env2.field_trials(), Ref(field_trials2));
}

TEST(MediaContextTest, FactoryCanCreateNewMediaContextFromExistingOne)
{
    MediaContext env1 = CreateMediaContext(std::make_unique<FakeTaskQueueFactory>());
    MediaContextFactory factory(env1);
    factory.Set(std::make_unique<FakeFieldTrials>());
    MediaContext env2 = factory.Create();

    // MediaContexts share the same default clock.
    EXPECT_THAT(env2.clock(), Ref(env1.clock()));

    // MediaContexts share the same custom task queue factory.
    EXPECT_THAT(env2.task_queue_factory(), Ref(env1.task_queue_factory()));

    // MediaContexts have different field trials.
    EXPECT_THAT(env2.field_trials(), Not(Ref(env1.field_trials())));
}

TEST(MediaContextTest, KeepsOwnershipsWhenCreateNewMediaContextFromExistingOne)
{
    bool utility1_destroyed = false;
    bool utility2_destroyed = false;
    Optional<MediaContext> env1 = CreateMediaContext(std::make_unique<FakeTaskQueueFactory>(
        /*on_destroyed=*/[&] { utility1_destroyed = true; }));

    Optional<MediaContextFactory> factory = MediaContextFactory(*env1);

    // Destroy env1, check utility1 it was using is still alive.
    env1 = utils::nullopt;
    EXPECT_FALSE(utility1_destroyed);

    factory->Set(std::make_unique<FakeFieldTrials>(
        /*on_destroyed=*/[&] { utility2_destroyed = true; }));
    Optional<MediaContext> env2 = factory->Create();

    // Destroy the factory, check all utilities used by env2 are alive.
    factory = utils::nullopt;
    EXPECT_FALSE(utility1_destroyed);
    EXPECT_FALSE(utility2_destroyed);

    // Once last MediaContext object is deleted, utilties should be deleted too.
    env2 = utils::nullopt;
    EXPECT_TRUE(utility1_destroyed);
    EXPECT_TRUE(utility2_destroyed);
}

TEST(MediaContextTest, DestroysUtilitiesInReverseProvidedOrder)
{
    std::vector<std::string> destroyed;
    auto field_trials = std::make_unique<FakeFieldTrials>(
        /*on_destroyed=*/[&] { destroyed.push_back("field_trials"); });
    auto task_queue_factory = std::make_unique<FakeTaskQueueFactory>(
        /*on_destroyed=*/[&] { destroyed.push_back("task_queue_factory"); });

    Optional<MediaContext> env = CreateMediaContext(std::move(field_trials), std::move(task_queue_factory));

    ASSERT_THAT(destroyed, IsEmpty());
    env = utils::nullopt;
    EXPECT_THAT(destroyed, ElementsAre("task_queue_factory", "field_trials"));
}

} // namespace

OCTK_END_NAMESPACE
